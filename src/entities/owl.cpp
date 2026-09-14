#include "dispatch.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "attacks.hpp"
#include "bird_feeding.hpp"

namespace {

void settle(Entity& owl) {
    owl.label_a = 3;
    owl.timer_a = 36;
    owl.move_interval = 16;
    owl.sprite = Sprite::Owl;
}

void swoop(Game& game, int slot) {
    Entity& owl = game.entities[static_cast<std::size_t>(slot)];
    if (owl.move_wait > 0) return;
    if (owl.timer_a == 0 || owl.cell == owl.point_b) { settle(owl); return; }
    const auto next = next_route_cell(game, slot, owl.point_b, 256);
    if (!next) { settle(owl); return; }
    const int victim = entity_at(game, *next, true);
    if (victim >= 0) {
        damage_entity(game, victim, 18, owl.cell);
        emit_sound(game, SoundId::OwlClaw, owl.cell);
        settle(owl);
    } else if (!move_entity(game, slot, *next)) settle(owl);
}

} // namespace

// SLOTS: point_a perch/territory; point_b old target cell; label_a watch/tell/dive/
// rest/return; timer_a phase. A fleeing player does not drag its landing marker.
void init_owl(Entity& owl) {
    owl.sprite = Sprite::Owl;
    owl.health = owl.max_health = 38;
    owl.move_interval = 16;
    owl.impassable = true;
    owl.point_a = owl.cell;
    owl.timer_a = 90;
}

void step_owl(Game& game, int slot) {
    Entity& owl = game.entities[static_cast<std::size_t>(slot)];
    if (owl.label_a == 1) {
        if (owl.timer_a > 0) return;
        owl.label_a = 2; owl.timer_a = 60; owl.move_interval = 4; owl.move_wait = 0;
        owl.sprite = Sprite::OwlFlying;
        emit_sound(game, SoundId::OwlSwoop, owl.cell);
        return;
    }
    if (owl.label_a == 2) { swoop(game, slot); return; }
    if (owl.label_a == 3) {
        if (owl.timer_a == 0) owl.label_a = 4;
        return;
    }
    if (step_hearing(game, slot)) return;
    if (owl.label_a == 4) {
        if (distance(owl.cell, owl.point_a) <= 1) { owl.label_a = 0; owl.timer_a = 150; }
        else pursue(game, slot, owl.point_a);
        return;
    }
    if (feed_on_bird_seed(game, slot)) return;
    if (owl.timer_a > 0) return;
    const int target = nearest_player(game, owl.point_a, 6);
    if (target < 0) return;
    const Cell cell = game.entities[static_cast<std::size_t>(target)].cell;
    if (!clear_sight(game, owl.cell, cell)) return;
    owl.point_b = cell;
    owl.facing = cardinal_toward(owl.cell, cell, owl.facing);
    owl.label_a = 1; owl.timer_a = 40;
    emit_sound(game, SoundId::OwlHoot, owl.cell);
}
