#include "behavior.hpp"
#include "wolf.hpp"
#include "wolf_call.hpp"
#include "hearing.hpp"
#include "../surfaces/interaction.hpp"
#include "foraging.hpp"
#include "dispatch.hpp"
#include "attacks.hpp"

namespace {

// SLOTS: entity_a pack leader, entity_b prey; point_a home, point_b locked bite;
// label_a hunt/bite/recover; timer_a bite/recover; timer_b pack/howl beat.
// WhistleHunt uses c attention time and entity_b prey; food retains its b slots.
void find_pack(Game& game, int slot) {
    Entity& wolf = game.entities[static_cast<std::size_t>(slot)];
    wolf.entity_a = {};
    // ORDER: Lower slots lead, so a pack cannot form a follower cycle.
    for (int index = 0; index < slot; ++index) {
        const Entity& other = game.entities[static_cast<std::size_t>(index)];
        if (other.kind == EntityKind::Wolf && other.health > 0 && distance(wolf.cell, other.cell) <= 7) {
            wolf.entity_a = {index, other.generation};
            break;
        }
    }
    wolf.timer_b = 180;
}

} // namespace

void interrupt_wolf_bite(Entity& wolf) {
    if (wolf.kind!=EntityKind::Wolf || wolf.label_a!=WolfBiteWindup) return;
    wolf.label_a=WolfRecover;wolf.timer_a=wolf_recovery_ticks;
    wolf.sprite=Sprite::Wolf;wolf.use_flash=0;
}

bool valid_wolf_bite(const Entity& wolf) {
    return wolf.kind!=EntityKind::Wolf || (wolf.label_a>=WolfHunt && wolf.label_a<=WolfRecover &&
        wolf.timer_a>=0 && wolf.timer_a<=wolf_recovery_ticks &&
        (wolf.label_a!=WolfBiteWindup || distance({},wolf.facing)==1));
}

void init_wolf(Entity& wolf) {
    wolf.sprite = Sprite::Wolf;
    wolf.health = wolf.max_health = 45;
    wolf.move_interval = 12;
    wolf.impassable = true;
    wolf.point_a = wolf.cell;
}

void step_wolf(Game& game, int slot) {
    Entity& wolf = game.entities[static_cast<std::size_t>(slot)];
    const Entity* called_prey = called_wolf_prey(game, wolf);
    if (!called_prey && step_foraging(game, slot, wolf.label_a != 0)) return;
    if (wolf.label_a == WolfBiteWindup) {
        // The locked target and facing also identify the original attack cell.
        if (wolf.cell+wolf.facing!=wolf.point_b || wolf.vitals.rooted>0 ||
            wolf.stun_ticks>0 || wolf.sleep_ticks>0 || wolf.toss.ticks>0) {
            interrupt_wolf_bite(wolf);return;
        }
        if (wolf.timer_a == 0) {
            resolve_enemy_attack(game, slot, 11, SoundId::WolfSnap);
            wolf.sprite=Sprite::WolfLunge;
            wolf.label_a = WolfRecover;
            wolf.timer_a = wolf_recovery_ticks;
        }
        return;
    }
    if (wolf.label_a == WolfRecover) {
        if (wolf.use_flash==0) wolf.sprite=Sprite::Wolf;
        if (wolf.timer_a == 0) wolf.label_a = WolfHunt;
        return;
    }
    if (!called_prey && step_hearing(game, slot)) return;
    if (!called_prey && wolf.timer_b == 0) find_pack(game, slot);
    const Entity* leader = get_entity(game, wolf.entity_a);
    auto target = called_prey ? std::optional<EnemyTarget>{{called_prey->cell, wolf.entity_b}} :
        enemy_target(game, wolf.cell, 8);
    if (!called_prey && leader != nullptr && leader->health > 0 && (!target || target->actor.slot >= 0)) {
        const Entity* prey = get_entity(game, leader->entity_b);
        if (prey != nullptr && prey->health > 0 && prey->owner >= 0 && prey->owner < 4 &&
            game.run.online[static_cast<std::size_t>(prey->owner)] && distance(wolf.cell, prey->cell) < 12 &&
            !smoke_hides(game.stage, wolf.cell, prey->cell))
            target = EnemyTarget{prey->cell, leader->entity_b};
    }
    if (!target) {
        wolf.entity_b = {};
        if (distance(wolf.cell, wolf.point_a) > 5) pursue(game, slot, wolf.point_a);
        else if (game.tick % 12 == 0) wander(game, slot);
        return;
    }
    const Cell prey = target->cell;
    const bool acquired = target->actor.slot >= 0 && get_entity(game, wolf.entity_b) == nullptr;
    wolf.entity_b = target->actor;
    if (acquired && leader == nullptr) emit_sound(game, SoundId::WolfHowl, wolf.cell);
    if (distance(wolf.cell, prey) == 1 && clear_attack_sight(game, wolf.cell, prey)) {
        wolf.point_b = prey;
        wolf.facing = cardinal_toward(wolf.cell, prey, wolf.facing);
        wolf.label_a = WolfBiteWindup;
        wolf.timer_a = wolf_windup_ticks;
        wolf.sprite=Sprite::WolfCrouch;
        emit_sound(game,SoundId::WolfSnarl,wolf.cell);
        return;
    }
    Cell destination = prey;
    if (leader != nullptr && distance(wolf.cell, prey) > 2) {
        const Cell line = cardinal_toward(leader->cell, prey, leader->facing);
        const int flank = (slot & 1) == 0 ? 1 : -1;
        const Cell candidate = prey + Cell{-line.y * flank, line.x * flank};
        const Tile* tile = game.stage.at(candidate);
        if (tile != nullptr && walkable(*tile) && entity_at(game, candidate, true) < 0)
            destination = candidate;
    }
    pursue(game, slot, destination);
}
