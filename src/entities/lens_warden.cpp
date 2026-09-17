#include "lens_warden.hpp"
#include "behavior.hpp"
#include "../projectiles/projectile.hpp"
#include "../props/interaction.hpp"

#include <algorithm>

namespace {

// SLOTS: label_a phase; label_b chosen mirror angle; timer_a phase; timer_b search beat.
// point_a lamp, point_b mirror; counter_a/b home x/y; counter_c pre-turn mirror angle.
// No hearing state is used; generic c-slot expiration may clear point_c safely.
constexpr int beam_damage = 26, beam_reach = 12;
Cell home(const Entity& warden) { return {warden.counter_a, warden.counter_b}; }

void recover(Entity& warden, int ticks) {
    warden.label_a = WardenRecover;
    warden.timer_a = ticks;
    warden.sprite = Sprite::WardenRecover;
}

bool apparatus(const Game& game, const Entity& warden) {
    const Tile* lamp = game.stage.at(warden.point_a);
    const Tile* mirror = game.stage.at(warden.point_b);
    return lamp && mirror && lamp->prop.kind == PropKind::BeamLamp && !lamp->prop.broken && !lamp->prop.covered &&
        mirror->prop.kind == PropKind::MirrorShard && !mirror->prop.broken && !mirror->prop.covered &&
        distance(home(warden), warden.point_b) == 1;
}

bool lamp_path(const Game& game, const Entity& warden) {
    if (!apparatus(game, warden)) return false;
    const Cell from = warden.point_a, to = warden.point_b;
    if (from.x != to.x && from.y != to.y) return false;
    const int length = distance(from, to);
    if (length < 2 || length > 4 || projectile_blocked(game, from)) return false;
    const Cell direction = cardinal_toward(from, to, {1, 0});
    for (int i = 1; i <= length; ++i) {
        const Cell cell = from + Cell{direction.x * i, direction.y * i};
        if (projectile_blocked(game, cell)) return false;
        if (i < length && (optical_prop(game.stage.at(cell)->prop) || game.stage.at(cell)->prop.covered)) return false;
    }
    return true;
}

bool ready_lamp(const Game& game, const Entity& warden) {
    return warden.kind == EntityKind::LensWarden && warden.health > 0 &&
        warden.label_a == WardenCharge &&
        warden.cell == home(warden) && warden.sleep_ticks == 0 && warden.stun_ticks == 0 &&
        lamp_path(game, warden) && game.stage.at(warden.point_b)->prop.variant % 2 == warden.label_b;
}

bool reaches_player(const Game& game, const BeamTrace& beam) {
    for (int i = 0; i < beam.count; ++i) {
        const Handle hit = beam.cells[static_cast<std::size_t>(i)].target;
        const Entity* actor = get_entity(game, hit);
        if (!actor || actor->kind != EntityKind::Player || actor->health <= 0 ||
            actor->owner < 0 || !has_player(game, actor->owner)) continue;
        if (player_state(game, actor->owner).online) return true;
    }
    return false;
}

void choose_lane(Game& game, Entity& warden) {
    if (warden.timer_b > 0 || !lamp_path(game, warden)) return;
    warden.timer_b = 24;
    const Cell incoming = cardinal_toward(warden.point_a, warden.point_b, {1, 0});
    for (int angle = 0; angle < 2; ++angle) {
        const Cell out = angle == 0 ? Cell{-incoming.y, -incoming.x} : Cell{incoming.y, incoming.x};
        if (!reaches_player(game, trace_beam(game, warden.point_b, out, beam_damage, beam_reach, false))) continue;
        warden.counter_c = game.stage.at(warden.point_b)->prop.variant % 2;
        warden.label_b = angle;
        warden.label_a = WardenTurn;
        warden.timer_a = 30;
        warden.facing = cardinal_toward(warden.cell, warden.point_b, warden.facing);
        warden.sprite = Sprite::WardenTurn;
        emit_sound(game, SoundId::WardenTurn, warden.cell);
        return;
    }
}

} // namespace

void init_lens_warden(Entity& warden) {
    warden.health = warden.max_health = 64;
    warden.move_interval = 26;
    warden.impassable = true;
    warden.sprite = Sprite::LensWarden;
    warden.counter_a = warden.cell.x; warden.counter_b = warden.cell.y;
}

void interrupt_lens_warden(Entity& warden) {
    if (warden.kind == EntityKind::LensWarden && warden.health > 0 &&
        (warden.label_a == WardenTurn || warden.label_a == WardenCharge)) recover(warden, 90);
}

bool warden_charging(const Game& game, const Entity& warden) { return ready_lamp(game, warden); }

BeamTrace warden_beam(const Game& game, const Entity& warden) {
    if (!ready_lamp(game, warden)) return {};
    const Cell direction = cardinal_toward(warden.point_a, warden.point_b, {1, 0});
    return trace_beam(game, warden.point_a, direction, beam_damage, beam_reach, false);
}

std::vector<Cell> charged_warden_lamps(const Game& game) {
    std::vector<Cell> cells;
    for (const Entity& actor : game.entities)
        if (ready_lamp(game, actor)) cells.push_back(actor.point_a);
    return cells;
}

void step_lens_warden(Game& game, int slot) {
    Entity& warden = game.entities[static_cast<std::size_t>(slot)];
    if (warden.cell != home(warden) || !lamp_path(game, warden)) interrupt_lens_warden(warden);
    if (warden.label_a == WardenRecover) {
        if (warden.timer_a == 0) { warden.label_a = WardenIdle; warden.sprite = Sprite::LensWarden; }
        return;
    }
    if (warden.label_a == WardenTurn) {
        // INTERVENTION: A player turning the mirror during our turn cancels the job.
        if (game.stage.at(warden.point_b)->prop.variant % 2 != warden.counter_c) {
            recover(warden, 90); return;
        }
        if (warden.timer_a > 0) return;
        game.stage.at(warden.point_b)->prop.variant = static_cast<std::uint8_t>(warden.label_b);
        warden.label_a = WardenCharge;
        warden.timer_a = 48;
        warden.sprite = Sprite::WardenCharge;
        emit_sound(game, SoundId::WardenCharge, warden.point_a);
        return;
    }
    if (warden.label_a == WardenCharge) {
        if (!ready_lamp(game, warden)) { recover(warden, 90); return; }
        if (warden.timer_a > 0) return;
        const BeamTrace beam = warden_beam(game, warden);
        // COMMITMENT: The mirror angle stays fixed; new obstacles still stop the real beam.
        resolve_beam(game, beam);
        emit_sound(game, SoundId::WardenFire, warden.point_a);
        if (warden.health > 0) recover(warden, 120);
        return;
    }
    if (warden.cell != home(warden)) {
        if (warden.move_wait == 0)
            if (const auto next = next_route_cell(game, slot, home(warden), 256)) willing_step(game, slot, *next);
        return;
    }
    choose_lane(game, warden);
}
