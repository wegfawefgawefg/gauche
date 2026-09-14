#include "behavior.hpp"
#include "hearing.hpp"
#include "../surfaces/interaction.hpp"
#include "foraging.hpp"
#include "dispatch.hpp"
#include "attacks.hpp"

namespace {

// SLOTS: entity_a pack leader, entity_b prey; point_a home, point_b locked bite;
// label_a hunt/bite/recover; timer_a bite/recover; timer_b pack/howl beat.
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

void init_wolf(Entity& wolf) {
    wolf.sprite = Sprite::Wolf;
    wolf.health = wolf.max_health = 45;
    wolf.move_interval = 12;
    wolf.impassable = true;
    wolf.point_a = wolf.cell;
}

void step_wolf(Game& game, int slot) {
    Entity& wolf = game.entities[static_cast<std::size_t>(slot)];
    if (step_foraging(game, slot, wolf.label_a != 0)) return;
    if (wolf.label_a == 1) {
        if (wolf.timer_a == 0) {
            resolve_enemy_attack(game, slot, 11, SoundId::WolfBite);
            wolf.label_a = 2;
            wolf.timer_a = 32;
        }
        return;
    }
    if (wolf.label_a == 2) {
        if (wolf.timer_a == 0) wolf.label_a = 0;
        return;
    }
    if (step_hearing(game, slot)) return;
    if (wolf.timer_b == 0) find_pack(game, slot);
    const Entity* leader = get_entity(game, wolf.entity_a);
    int target = nearest_player(game, wolf.cell, 8);
    if (leader != nullptr && leader->health > 0) {
        const Entity* prey = get_entity(game, leader->entity_b);
        if (prey != nullptr && prey->health > 0 && prey->owner >= 0 && prey->owner < 4 &&
            game.run.online[static_cast<std::size_t>(prey->owner)] && distance(wolf.cell, prey->cell) < 12 &&
            !smoke_hides(game.stage, wolf.cell, prey->cell))
            target = leader->entity_b.slot;
    }
    if (target < 0) {
        wolf.entity_b = {};
        if (distance(wolf.cell, wolf.point_a) > 5) pursue(game, slot, wolf.point_a);
        else if (game.tick % 12 == 0) wander(game, slot);
        return;
    }
    const Entity& prey = game.entities[static_cast<std::size_t>(target)];
    const bool acquired = get_entity(game, wolf.entity_b) == nullptr;
    wolf.entity_b = {target, prey.generation};
    if (acquired && leader == nullptr) emit_sound(game, SoundId::WolfHowl, wolf.cell);
    if (distance(wolf.cell, prey.cell) == 1 && clear_sight(game, wolf.cell, prey.cell)) {
        wolf.point_b = prey.cell;
        wolf.facing = cardinal_toward(wolf.cell, prey.cell, wolf.facing);
        wolf.label_a = 1;
        wolf.timer_a = 18;
        return;
    }
    Cell destination = prey.cell;
    if (leader != nullptr && distance(wolf.cell, prey.cell) > 2) {
        const Cell line = cardinal_toward(leader->cell, prey.cell, leader->facing);
        const int flank = (slot & 1) == 0 ? 1 : -1;
        const Cell candidate = prey.cell + Cell{-line.y * flank, line.x * flank};
        const Tile* tile = game.stage.at(candidate);
        if (tile != nullptr && walkable(*tile) && entity_at(game, candidate, true) < 0)
            destination = candidate;
    }
    pursue(game, slot, destination);
}
