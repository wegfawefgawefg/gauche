#include "bell_diver.hpp"
#include "diver_route.hpp"
#include "behavior.hpp"
#include "attacks.hpp"

namespace {

// SLOTS: label_a phase; timer_a phase duration; counter_a surfaced hunting time.
// point_a remembered air hole; point_b committed swing cell; facing marks that swing.
void sink(Game& game, Entity& diver) {
    diver.label_a = DiverSink;
    diver.timer_a = 24;
    diver.sprite = Sprite::DiverSink;
    emit_sound(game, SoundId::DiverDive, diver.cell);
}

void swim(Game& game, int slot) {
    Entity& diver = game.entities[static_cast<std::size_t>(slot)];
    diver.burn_ticks = diver.scorch_ticks = 0;
    if (diver.move_wait > 0 || diver.vitals.rooted > 0) return;
    const int player = nearest_player(game, diver.cell, 12);
    const Cell target = player < 0 ? diver.point_a : game.entities[static_cast<std::size_t>(player)].cell;
    const DiverRoute route = diver_route(game, slot, target);
    diver.move_wait = 18;
    if (!route.found) return;
    diver.point_a = route.hole;
    if (diver.cell != route.hole) {
        diver.facing = route.next - diver.cell;
        diver.cell = route.next;
        // SURFACE: Bubbles occupy the actual cell; submerged travel does not trample props.
        emit_sound(game, SoundId::DiverBubbles, diver.cell);
        return;
    }
    if (player < 0 || distance(diver.cell, target) > 7 || !clear_attack_sight(game, diver.cell, target)) return;
    diver.label_a = DiverBell;
    diver.timer_a = 36;
    diver.sprite = Sprite::DiverBell;
    emit_sound(game, SoundId::DiverRing, diver.cell);
}

void hunt(Game& game, int slot) {
    Entity& diver = game.entities[static_cast<std::size_t>(slot)];
    if (diver.counter_a > 0) --diver.counter_a;
    const auto target = enemy_target(game, diver.cell, 5);
    if (target && diver.counter_a > 0 && distance(diver.cell, target->cell) == 1 &&
        clear_attack_sight(game, diver.cell, target->cell)) {
        diver.point_b = target->cell;
        diver.facing = target->cell - diver.cell;
        diver.label_a = DiverSwing;
        diver.timer_a = 30;
        diver.sprite = Sprite::DiverSwing;
        emit_sound(game, SoundId::DiverWindup, diver.cell);
        return;
    }
    if (target && diver.counter_a > 0 && distance(diver.cell, diver.point_a) < 3 &&
        clear_attack_sight(game, diver.cell, target->cell)) {
        pursue(game, slot, target->cell);
        return;
    }
    if (game.stage.at_or_border(diver.cell).kind == TileKind::IceHole) { sink(game, diver); return; }
    if (diver.move_wait > 0) return;
    // RETURN: A blocked remembered hole is replaced by another reachable entrance.
    // If none is open, the exposed diver remains vulnerable and fights on land.
    Cell best = diver.cell;
    int gap = 100;
    for (int y = -6; y <= 6; ++y)
        for (int x = -6; x <= 6; ++x) {
            const Cell cell = diver.cell + Cell{x, y};
            const int length = distance(diver.cell, cell);
            if (length >= gap || !diver_hole_open(game, cell, slot)) continue;
            if (next_route_cell(game, slot, cell, 256)) { best = cell; gap = length; }
        }
    if (best == diver.cell) { diver.counter_a = 90; diver.move_wait = 18; return; }
    diver.point_a = best;
    if (const auto next = next_route_cell(game, slot, best, 256)) willing_step(game, slot, *next);
}

} // namespace

bool diver_submerged(const Entity& diver) {
    return diver.kind == EntityKind::BellDiver &&
        (diver.label_a == DiverSwim || diver.label_a == DiverBell);
}

void init_bell_diver(Entity& diver) {
    diver.health = diver.max_health = 72;
    diver.move_interval = 18;
    diver.impassable = false;
    diver.sprite = Sprite::DiverBubbles;
    diver.point_a = diver.cell;
}

void step_bell_diver(Game& game, int slot) {
    Entity& diver = game.entities[static_cast<std::size_t>(slot)];
    switch (diver.label_a) {
    case DiverSwim: swim(game, slot); return;
    case DiverBell:
        if (!diver_hole_open(game, diver.cell, slot)) {
            diver.label_a = DiverSwim; diver.sprite = Sprite::DiverBubbles;
            return;
        }
        if (diver.timer_a > 0) return;
        diver.label_a = DiverRise; diver.timer_a = 24;
        diver.impassable = true; diver.sprite = Sprite::DiverRise;
        emit_sound(game, SoundId::DiverRise, diver.cell);
        return;
    case DiverRise:
        if (diver.timer_a > 0) return;
        diver.label_a = DiverHunt; diver.counter_a = 150;
        diver.sprite = Sprite::BellDiver;
        return;
    case DiverHunt: hunt(game, slot); return;
    case DiverSwing:
        if (diver.timer_a > 0) return;
        if (distance(diver.cell, diver.point_b) == 1)
            resolve_enemy_attack(game, slot, 18, SoundId::DiverStrike);
        diver.label_a = DiverRecover; diver.timer_a = 48;
        diver.sprite = Sprite::BellDiver;
        return;
    case DiverRecover:
        if (diver.timer_a > 0) return;
        diver.label_a = DiverHunt; diver.counter_a = 0;
        return;
    case DiverSink:
        // INTERRUPT: Being pushed off the hole leaves the body exposed.
        if (game.stage.at_or_border(diver.cell).kind != TileKind::IceHole) {
            diver.label_a = DiverHunt; diver.counter_a = 60; diver.sprite = Sprite::BellDiver;
            return;
        }
        if (diver.timer_a > 0) return;
        diver.label_a = DiverSwim; diver.impassable = false;
        diver.sprite = Sprite::DiverBubbles; diver.move_wait = 36;
        return;
    default: return;
    }
}
