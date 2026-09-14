#include "../props/interaction.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "foraging.hpp"
#include "dispatch.hpp"
#include "attacks.hpp"
#include "../combat/shove.hpp"

namespace {

// SLOTS: label_a idle/windup/charge/recover; counter_a remaining lane;
// timer_a windup/recovery; point_a territory. Facing locks when it snorts.
enum { Idle, Windup, Charge, Recover };

void stop_charge(Entity& boar) {
    boar.label_a = Recover;
    boar.timer_a = 55;
    boar.move_interval = 22;
    boar.stun_ticks = 18;
}

void charge(Game& game, int slot) {
    Entity& boar = game.entities[static_cast<std::size_t>(slot)];
    if (boar.move_wait > 0) return;
    const Cell next = boar.cell + boar.facing;
    const Tile* tile = game.stage.at(next);
    if (tile != nullptr && prop_blocks(tile->prop)) {
        hit_prop(game, next, 18, boar.cell);
        emit_sound(game, SoundId::BoarHit, boar.cell);
        stop_charge(boar);
        return;
    }
    const int victim_slot = entity_at(game, next, true);
    if (victim_slot >= 0) {
        Entity& victim = game.entities[static_cast<std::size_t>(victim_slot)];
        const bool blocked = blocks_facing(victim, boar.cell) || victim.hard_blocker;
        damage_entity(game, victim_slot, 18, boar.cell);
        if (!blocked && victim.health > 0) shove_actor(game, victim_slot, boar.facing, boar.cell);
        emit_sound(game, SoundId::BoarHit, boar.cell);
        stop_charge(boar);
        return;
    }
    if (!move_entity(game, slot, next)) {
        emit_sound(game, SoundId::BoarHit, boar.cell);
        stop_charge(boar);
    } else if (--boar.counter_a <= 0) stop_charge(boar);
}

} // namespace

void init_boar(Entity& boar) {
    boar.sprite = Sprite::Boar;
    boar.health = boar.max_health = 65;
    boar.move_interval = 22;
    boar.impassable = true;
    boar.point_a = boar.cell;
}

void step_boar(Game& game, int slot) {
    Entity& boar = game.entities[static_cast<std::size_t>(slot)];
    if (step_foraging(game, slot, boar.label_a != Idle)) return;
    if (boar.label_a == Recover) {
        if (boar.timer_a == 0) boar.label_a = Idle;
        return;
    }
    if (boar.label_a == Windup) {
        if (boar.timer_a == 0) { boar.label_a = Charge; boar.move_interval = 4; boar.move_wait = 0; }
        return;
    }
    if (boar.label_a == Charge) { charge(game, slot); return; }
    if (step_hearing(game, slot)) return;
    const auto target = enemy_target(game, boar.cell, 7);
    if (!target) {
        if (distance(boar.cell, boar.point_a) > 4) pursue(game, slot, boar.point_a);
        else wander(game, slot);
        return;
    }
    const Cell target_cell = target->cell;
    if ((target_cell.x == boar.cell.x || target_cell.y == boar.cell.y) &&
        clear_attack_sight(game, boar.cell, target_cell)) {
        boar.facing = cardinal_toward(boar.cell, target_cell, boar.facing);
        boar.label_a = Windup;
        boar.timer_a = 36;
        boar.counter_a = 6;
        emit_sound(game, SoundId::BoarSnort, boar.cell);
    } else approach(game, slot, target_cell);
}
