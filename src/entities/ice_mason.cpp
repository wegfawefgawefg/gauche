#include "ice_mason.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "attacks.hpp"
#include "../props/ice_cover.hpp"
#include "../surfaces/temperature.hpp"

#include <algorithm>

namespace {

// SLOTS: label_a phase; counter_a carried block; timer_a phase; timer_b quarry search.
// point_a stand/origin; point_b quarry, placement or committed strike cell. c-slots are hearing.
constexpr Cell sides[]{{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

void rest(Entity& mason, int ticks) {
    mason.label_a = MasonRest;
    mason.timer_a = ticks;
    mason.sprite = mason.counter_a > 0 ? Sprite::MasonCarry : Sprite::IceMason;
}

bool quarry_ice(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    return tile && tile->kind == TileKind::Ice && tile->freeze_ticks == 0 &&
        (tile->prop.kind == PropKind::None || tile->prop.broken) &&
        entity_at(game, cell, false) < 0 && !warm_cell(game, cell);
}

bool stand_space(const Game& game, Cell cell, Cell origin) {
    const Tile* tile = game.stage.at(cell);
    return tile && walkable(*tile) && tile->kind != TileKind::Lava &&
        (cell == origin || entity_at(game, cell, true) < 0);
}

bool choose_quarry(Game& game, int slot, Cell threat) {
    Entity& mason = game.entities[static_cast<std::size_t>(slot)];
    int best = -10000;
    Cell source{}, stand{};
    // QUARRY: Cut native ice only. Temporary lids and diver holes retain their identity.
    for (int y = -6; y <= 6; ++y)
        for (int x = -6; x <= 6; ++x) {
            const Cell cell = mason.cell + Cell{x, y};
            const Tile* tile = game.stage.at(cell);
            if (!tile || tile->kind != TileKind::Ice || tile->freeze_ticks > 0 ||
                distance(cell, mason.cell) > 7 || !quarry_ice(game, cell)) continue;
            for (Cell side : sides) {
                const Cell at = cell + side;
                if (!stand_space(game, at, mason.cell) || !clear_sight(game, mason.cell, at)) continue;
                const int score = std::min(distance(at, threat), 6) * 2 - distance(at, mason.cell) * 3;
                if (score > best) { best = score; source = cell; stand = at; }
            }
        }
    if (best == -10000) return false;
    mason.point_a = stand;
    mason.point_b = source;
    mason.label_a = MasonSeek;
    mason.timer_a = 150;
    return true;
}

void seek_quarry(Game& game, int slot) {
    Entity& mason = game.entities[static_cast<std::size_t>(slot)];
    if (mason.timer_a == 0 || !quarry_ice(game, mason.point_b)) { rest(mason, 30); return; }
    if (mason.cell == mason.point_a) {
        mason.facing = cardinal_toward(mason.cell, mason.point_b, mason.facing);
        mason.label_a = MasonCut;
        mason.timer_a = 90;
        mason.sprite = Sprite::MasonCut;
        emit_sound(game, SoundId::MasonCut, mason.cell);
        return;
    }
    if (mason.move_wait > 0) return;
    const auto next = next_route_cell(game, slot, mason.point_a, 256);
    if (!next || !willing_step(game, slot, *next)) rest(mason, 30);
}

void finish_work(Game& game, Entity& mason) {
    if (mason.cell != mason.point_a) { rest(mason, 45); return; }
    if (mason.label_a == MasonCut && !quarry_ice(game, mason.point_b)) { rest(mason, 30); return; }
    if (mason.timer_a > 0) return;
    if (mason.label_a == MasonCut) {
        thaw_water(game, mason.point_b);
        mason.counter_a = 1;
        emit_sound(game, SoundId::MasonLift, mason.cell);
    } else if (place_ice_cover(game, mason.point_b)) mason.counter_a = 0;
    rest(mason, 24);
}

void ready(Game& game, int slot) {
    Entity& mason = game.entities[static_cast<std::size_t>(slot)];
    const auto target = enemy_target(game, mason.cell, 9);
    const bool seen = target && clear_attack_sight(game, mason.cell, target->cell);
    // DEFENSE: A short committed chisel jab creates space; the mason does not chase to melee.
    if (seen && distance(mason.cell, target->cell) == 1) {
        mason.point_a = mason.cell;
        mason.point_b = target->cell;
        mason.facing = cardinal_toward(mason.cell, target->cell, mason.facing);
        mason.label_a = MasonSwing;
        mason.timer_a = 24;
        mason.sprite = Sprite::MasonJab;
        emit_sound(game, SoundId::MasonWarn, mason.cell);
        return;
    }
    if (mason.counter_a == 0) {
        if (mason.timer_b > 0) return;
        mason.timer_b = 45;
        if (!choose_quarry(game, slot, seen ? target->cell : mason.cell)) {
            if (seen) flee(game, slot, target->cell);
            else if (!step_hearing(game, slot)) wander(game, slot);
        }
        return;
    }
    if (seen && distance(mason.cell, target->cell) <= 6) {
        mason.facing = cardinal_toward(mason.cell, target->cell, mason.facing);
        const Cell cell = mason.cell + mason.facing;
        if (ice_cover_space(game, cell)) {
            mason.label_a = MasonBuild;
            mason.timer_a = 30;
            mason.point_a = mason.cell;
            mason.point_b = cell;
            mason.sprite = Sprite::MasonBuild;
            emit_sound(game, SoundId::MasonSet, mason.cell);
        } else flee(game, slot, target->cell);
    } else if (!step_hearing(game, slot)) wander(game, slot);
}

} // namespace

void init_ice_mason(Entity& mason) {
    mason.health = mason.max_health = 60;
    mason.move_interval = 24;
    mason.impassable = true;
    mason.counter_a = 1;
    mason.sprite = Sprite::MasonCarry;
}

void interrupt_ice_mason(Entity& mason) {
    if (mason.kind == EntityKind::IceMason && (mason.label_a == MasonCut ||
        mason.label_a == MasonBuild || mason.label_a == MasonSwing)) rest(mason, 45);
}

void step_ice_mason(Game& game, int slot) {
    Entity& mason = game.entities[static_cast<std::size_t>(slot)];
    switch (mason.label_a) {
    case MasonRest:
        if (mason.timer_a == 0) mason.label_a = MasonReady;
        return;
    case MasonSeek: seek_quarry(game, slot); return;
    case MasonCut: case MasonBuild: finish_work(game, mason); return;
    case MasonSwing:
        if (mason.cell != mason.point_a) { rest(mason, 45); return; }
        if (mason.timer_a == 0) {
            resolve_enemy_attack(game, slot, 12, SoundId::MasonJab);
            rest(mason, 42);
        }
        return;
    default: ready(game, slot); return;
    }
}
