#include "../props/interaction.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "dispatch.hpp"
#include "zombie.hpp"
#include "attacks.hpp"
#include <algorithm>

namespace {

bool zombie_kind(EntityKind kind) { return kind==EntityKind::Zombie || kind==EntityKind::ZombieStack; }
int windup(const Entity& actor) { return actor.kind==EntityKind::ZombieStack ? 30 : 24; }
int recovery(const Entity& actor) { return actor.attack_interval-windup(actor); }

std::optional<Cell> neighbor(const Game& game,const Entity& zombie,bool hunt_animals) {
    if (const auto dummy=enemy_target(game,zombie.cell,1,false)) return dummy->cell;
    for (int other_slot = 0; other_slot < max_entities; ++other_slot) {
        const Entity& target = game.entities[static_cast<std::size_t>(other_slot)];
        if (target.health <= 0 || distance(zombie.cell, target.cell) != 1) continue;
        const bool player = target.kind == EntityKind::Player && target.owner >= 0 &&
            has_player(game, target.owner) && player_state(game, target.owner).online;
        if (player || (hunt_animals && (target.kind==EntityKind::Chicken || target.kind==EntityKind::Bunny)))
            return target.cell;
    }
    return std::nullopt;
}

} // namespace

// SHARED SWIPE SLOTS: counter_b ready/windup/recovery, attack_wait full cycle;
// point_b committed target, facing committed direction (together imply origin).
// Stack counter_a/label_a/timer_a and survivor point_a/label_b/timer_b stay fall-owned.
bool begin_zombie_swipe(Game& game,int slot,bool hunt_animals) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.attack_wait>0) return false;
    const auto target=neighbor(game,actor,hunt_animals);
    if (!target || !clear_attack_sight(game,actor.cell,*target)) return false;
    actor.point_b=*target;actor.facing=*target-actor.cell;
    actor.counter_b=ZombieWindup;actor.attack_wait=actor.attack_interval;
    actor.sprite=Sprite::ZombieAngry;
    emit_sound(game,SoundId::ZombieGrowl2,actor.cell);
    return true;
}

void interrupt_zombie_swipe(Entity& actor) {
    if (!zombie_kind(actor.kind) || actor.counter_b!=ZombieWindup) return;
    actor.counter_b=ZombieRecover;actor.sprite=Sprite::Zombie;
    actor.attack_wait=std::min(actor.attack_wait,recovery(actor));
}

bool step_zombie_swipe(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.counter_b==ZombieReady) return false;
    if (actor.counter_b==ZombieWindup) {
        if (actor.cell+actor.facing!=actor.point_b || actor.vitals.rooted>0)
            interrupt_zombie_swipe(actor);
        else if (actor.attack_wait<=recovery(actor)) {
            resolve_enemy_attack(game,slot,actor.kind==EntityKind::ZombieStack ? 8 : 5,SoundId::ZombieScratch1);
            actor.counter_b=ZombieRecover;actor.sprite=Sprite::Zombie;
            actor.attack_wait=recovery(actor);
        }
    }
    if (actor.counter_b==ZombieRecover && actor.attack_wait==0) actor.counter_b=ZombieReady;
    return true;
}

bool valid_zombie_swipe(const Entity& actor) {
    if (!zombie_kind(actor.kind)) return true;
    return actor.counter_b>=ZombieReady && actor.counter_b<=ZombieRecover &&
        actor.attack_wait>=0 && actor.attack_wait<=85 &&
        (actor.counter_b!=ZombieWindup || distance({},actor.facing)==1);
}

void init_zombie(Game& game, Entity& entity) {
    entity.sprite = Sprite::Zombie;
    entity.health = entity.max_health = 40;
    entity.move_interval = 48;
    entity.move_wait = static_cast<int>(random_u32(game) % 48);
    entity.attack_interval = 60;
    entity.impassable = true;
}

// SLOTS: label_b/timer_b and point_a retain the brief fall from a toppled stack.
void step_zombie(Game& game, int slot) {
    Entity& zombie = game.entities[static_cast<std::size_t>(slot)];
    if (step_zombie_swipe(game,slot) || begin_zombie_swipe(game,slot,true)) return;
    const auto target = enemy_target(game, zombie.cell, zombie.encounter.slot >= 0 ? 60 : 6,
        zombie.encounter.slot >= 0);
    if (step_hearing(game, slot)) {
        maybe_growl(game, slot, SoundId::ZombieGrowl1);
        return;
    }
    if (target) pursue(game, slot, target->cell);
    else wander(game, slot);
    if (begin_zombie_swipe(game,slot,true)) return;
    maybe_growl(game, slot, (slot + static_cast<int>(zombie.generation)) % 2 == 0 ?
                        SoundId::ZombieGrowl1 : SoundId::ZombieGrowl2);
}
