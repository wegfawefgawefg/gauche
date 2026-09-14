#include "../props/interaction.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "dispatch.hpp"
#include "attacks.hpp"

#include <algorithm>

namespace {

// SLOTS: label_a circle/windup/dive/rest; counter_a dive steps;
// counter_b orbit corner; timer_a phase; point_a home; facing locks on dive.
void dive(Game& game, int slot) {
    Entity& bat = game.entities[static_cast<std::size_t>(slot)];
    if (bat.move_wait > 0) return;
    const Cell next = bat.cell + bat.facing;
    const int target_slot = entity_at(game, next, true);
    const Tile* tile = game.stage.at(next);
    if (tile != nullptr && prop_blocks(tile->prop)) {
        hit_prop(game, next, 7, bat.cell);
        emit_sound(game, SoundId::BatBite, bat.cell);
        bat.counter_a = 0;
    } else if (target_slot >= 0) {
        damage_entity(game, target_slot, 7, bat.cell);
        emit_sound(game, SoundId::BatBite, bat.cell);
        bat.counter_a = 0;
    } else if (!move_entity(game, slot, next)) bat.counter_a = 0;
    else --bat.counter_a;
    if (bat.counter_a <= 0) {
        bat.label_a = 3;
        bat.timer_a = 60;
        bat.move_interval = 18;
    }
}

} // namespace

void init_bat(Entity& bat) {
    bat.sprite = Sprite::Bat;
    bat.health = bat.max_health = 18;
    bat.move_interval = 18;
    bat.impassable = true;
    bat.point_a = bat.cell;
}

void step_bat(Game& game, int slot) {
    Entity& bat = game.entities[static_cast<std::size_t>(slot)];
    if (bat.label_a == 1) {
        if (bat.timer_a == 0) { bat.label_a = 2; bat.move_interval = 5; bat.move_wait = 0; }
        return;
    }
    if (bat.label_a == 2) { dive(game, slot); return; }
    if (bat.label_a == 3) {
        if (bat.timer_a == 0) bat.label_a = 0;
        return;
    }
    if (step_hearing(game, slot)) return;
    const auto target = enemy_target(game, bat.cell, 7);
    if (!target) {
        if (distance(bat.cell, bat.point_a) > 2) pursue(game, slot, bat.point_a);
        else if (game.tick % 45 == 0) wander(game, slot);
        return;
    }
    const Cell cell = target->cell;
    if (distance(bat.cell, cell) <= 4 && (cell.x == bat.cell.x || cell.y == bat.cell.y) &&
        clear_attack_sight(game, bat.cell, cell)) {
        bat.facing = cardinal_toward(bat.cell, cell, bat.facing);
        bat.label_a = 1;
        bat.timer_a = 24;
        bat.counter_a = 4;
        emit_sound(game, SoundId::BatSqueak, bat.cell);
        return;
    }
    constexpr Cell orbit[]{{-3, 0}, {0, -3}, {3, 0}, {0, 3}};
    const Cell destination = cell + orbit[static_cast<std::size_t>(bat.counter_b % 4)];
    if (distance(bat.cell, destination) <= 1) bat.counter_b = (bat.counter_b + 1) % 4;
    if (bat.move_wait == 0) approach(game, slot, destination);
}
