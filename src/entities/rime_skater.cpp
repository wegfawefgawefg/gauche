#include "rime_skater.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/slip.hpp"

#include <algorithm>

namespace {

// SLOTS: label_a idle/push/glide/brake; timer_a phase; counter_a remaining lane.
// point_a expected cell detects displacement; point_b is the committed heading.
enum { Idle, Push, Glide, Brake };

void brake(Game& game, Entity& skater, bool collision) {
    skater.label_a = Brake;
    skater.timer_a = collision ? 60 : 36;
    skater.move_interval = 24;
    skater.counter_a = 0;
    skater.sprite = Sprite::RimeSkater;
    if (collision) skater.stun_ticks = std::max(skater.stun_ticks, 18);
    emit_sound(game, SoundId::SkaterBrake, skater.cell);
}

void glide(Game& game, int slot) {
    Entity& skater = game.entities[static_cast<std::size_t>(slot)];
    if (skater.move_wait > 0) return;
    const Cell next = skater.cell + skater.point_b;
    const Tile* tile = game.stage.at(next);
    if (tile == nullptr || !walkable(*tile)) {
        if (tile != nullptr && prop_blocks(tile->prop)) hit_prop(game, next, 18, skater.cell);
        brake(game, skater, true);
        return;
    }
    const int victim = entity_at(game, next, true);
    if (victim >= 0) {
        damage_entity(game, victim, 18, skater.cell);
        emit_sound(game, SoundId::SkaterHit, skater.cell);
        brake(game, skater, true);
        return;
    }
    // LANDING: One real cell per beat; springs and doors take over the movement.
    if (!move_entity(game, slot, next, false)) { brake(game, skater, true); return; }
    if (skater.health <= 0) return;
    skater.point_a = next;
    if (skater.cell != next || !bare_ice(game.stage.at_or_border(next)) || --skater.counter_a <= 0)
        brake(game, skater, false);
}

void seek_ice(Game& game, int slot, Cell target) {
    Entity& skater = game.entities[static_cast<std::size_t>(slot)];
    if (skater.move_wait > 0) return;
    Cell best = skater.cell;
    int score = 1000;
    // POSITION: Return to a nearby lane rather than trying to skate across dry banks.
    for (int y = -5; y <= 5; ++y)
        for (int x = -5; x <= 5; ++x) {
            const Cell cell = skater.cell + Cell{x, y};
            const Tile* tile = game.stage.at(cell);
            if (tile == nullptr || !bare_ice(*tile) || !walkable(*tile) ||
                (cell != skater.cell && entity_at(game, cell, true) >= 0)) continue;
            const int alignment = cell.x == target.x || cell.y == target.y ? 0 : 5;
            const int candidate = distance(cell, skater.cell) + distance(cell, target) + alignment;
            if (candidate < score && clear_sight(game, skater.cell, cell)) { best = cell; score = candidate; }
        }
    if (best != skater.cell) approach(game, slot, best);
    else skater.move_wait = 24;
}

} // namespace

void init_rime_skater(Entity& skater) {
    skater.sprite = Sprite::RimeSkater;
    skater.health = skater.max_health = 50;
    skater.move_interval = 24;
    skater.impassable = true;
}

void step_rime_skater(Game& game, int slot) {
    Entity& skater = game.entities[static_cast<std::size_t>(slot)];
    if (skater.label_a == Brake) {
        if (skater.timer_a == 0) skater.label_a = Idle;
        return;
    }
    if (skater.label_a != Idle && (skater.cell != skater.point_a ||
        !bare_ice(game.stage.at_or_border(skater.cell)) || skater.vitals.rooted > 0)) {
        brake(game, skater, false);
        return;
    }
    if (skater.label_a == Push) {
        if (skater.timer_a == 0) {
            skater.label_a = Glide;
            skater.move_interval = 5;
            skater.move_wait = 0;
            skater.sprite = Sprite::RimeSkaterGlide;
        }
        return;
    }
    if (skater.label_a == Glide) { glide(game, slot); return; }
    if (skater.move_wait > 0 || skater.vitals.rooted > 0) return;
    const auto target = enemy_target(game, skater.cell, 10);
    if (!target || !clear_attack_sight(game, skater.cell, target->cell)) {
        skater.move_wait = 24;
        return;
    }
    const Cell destination = target->cell;
    if (bare_ice(game.stage.at_or_border(skater.cell)) &&
        (destination.x == skater.cell.x || destination.y == skater.cell.y)) {
        skater.facing = cardinal_toward(skater.cell, destination, skater.facing);
        skater.point_a = skater.cell;
        skater.point_b = skater.facing;
        skater.counter_a = 12;
        skater.label_a = Push;
        skater.timer_a = 30;
        skater.sprite = Sprite::RimeSkaterPush;
        emit_sound(game, SoundId::SkaterPush, skater.cell);
    } else seek_ice(game, slot, destination);
}
