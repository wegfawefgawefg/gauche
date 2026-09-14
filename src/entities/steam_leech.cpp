#include "steam_leech.hpp"
#include "../props/candle.hpp"
#include "../props/stove.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include "../surfaces/temperature.hpp"

#include <algorithm>

namespace {

// SLOTS: label_a phase; timer_a phase time; timer_b search delay; entity_a hot actor.
// label_b selects a terrain source; point_b terrain cell; point_a attachment origin.
void rest(Entity& leech, int ticks) {
    leech.label_a = LeechRest;
    leech.timer_a = ticks;
    leech.entity_a = {};
    leech.label_b = 0;
    leech.sprite = Sprite::LeechSpent;
}

bool find_heat(Game& game, Entity& leech) {
    int best = 1000;
    leech.entity_a = {};
    leech.label_b = 0;
    const auto consider = [&](Cell cell, Handle handle, bool surface) {
        const int gap = distance(leech.cell, cell);
        if (gap > 9 || gap >= best || !clear_attack_sight(game, leech.cell, cell, false)) return;
        best = gap;
        leech.entity_a = handle;
        leech.label_b = surface ? 1 : 0;
        leech.point_b = cell;
    };
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& source = game.entities[static_cast<std::size_t>(slot)];
        if (entity_has_flame(source)) consider(source.cell, {slot, source.generation}, false);
    }
    // SCENERY: A bounded search also finds capsule patches, burning oil and lava edges.
    for (int y = std::max(0, leech.cell.y - 9); y <= std::min(game.stage.height - 1, leech.cell.y + 9); ++y)
        for (int x = std::max(0, leech.cell.x - 9); x <= std::min(game.stage.width - 1, leech.cell.x + 9); ++x) {
            const Tile& tile = *game.stage.at({x, y});
            if (prop_has_flame(tile.prop) || tile.surface.warmth_ticks > 0 || tile.surface.fire_ticks > 0 || tile.kind == TileKind::Lava)
                consider({x, y}, {}, true);
        }
    leech.timer_b = 30;
    return best < 1000;
}

void seek(Game& game, int slot) {
    Entity& leech = game.entities[static_cast<std::size_t>(slot)];
    if (step_hearing(game, slot)) return;
    if (leech.timer_b == 0) find_heat(game, leech);
    Cell source;
    if (!leech_source_cell(game, leech, source)) { wander(game, slot); return; }
    if (distance(leech.cell, source) <= 1 && clear_attack_sight(game, leech.cell, source, false)) {
        leech.label_a = LeechLatch;
        leech.timer_a = 18;
        leech.point_a = leech.cell;
        leech.facing = cardinal_toward(leech.cell, source, leech.facing);
        leech.sprite = Sprite::LeechLatch;
        emit_sound(game, SoundId::LeechLatch, leech.cell);
        return;
    }
    if (leech.move_wait > 0) return;
    if (const auto next = next_route_cell(game, slot, source, 256)) willing_step(game, slot, *next);
    else { leech.timer_b = 0; wander(game, slot); }
}

} // namespace

void init_steam_leech(Entity& leech) {
    leech.sprite = Sprite::SteamLeech;
    leech.health = leech.max_health = 38;
    leech.self_light = {42, 27, 15};
    leech.move_interval = 18;
    leech.impassable = true;
    leech.point_a = leech.cell;
}

void release_steam_leech(Entity& leech, int ticks) {
    // RELEASE: Cold releases the source immediately, before adjacent warmth can clear chill.
    rest(leech, std::max(90, ticks));
}

void step_steam_leech(Game& game, int slot) {
    Entity& leech = game.entities[static_cast<std::size_t>(slot)];
    if (leech.label_a == LeechRest) {
        if (leech.timer_a == 0) { leech.label_a = LeechSeek; leech.sprite = Sprite::SteamLeech; leech.timer_b = 0; }
        return;
    }
    if (leech.label_a == LeechSeek) { seek(game, slot); return; }
    Cell source;
    // TETHER: Shoves, disappearing fuel and moving hosts cancel an attachment, never teleport it.
    if (leech.cell != leech.point_a || !leech_source_cell(game, leech, source) ||
        distance(leech.cell, source) > 1 || !clear_attack_sight(game, leech.cell, source, false)) {
        rest(leech, 60);
        emit_sound(game, SoundId::LeechDetach, leech.cell);
        return;
    }
    if (leech.timer_a > 0) return;
    switch (leech.label_a) {
    case LeechLatch:
        leech.label_a = LeechFeed;
        leech.timer_a = 120;
        leech.sprite = Sprite::LeechFeed;
        emit_sound(game, SoundId::LeechSip, leech.cell);
        break;
    case LeechFeed:
        leech.label_a = LeechSwell;
        leech.timer_a = 36;
        leech.sprite = Sprite::LeechSwell;
        emit_sound(game, SoundId::LeechWarning, leech.cell);
        break;
    case LeechSwell:
        resolve_enemy_attack(game, slot, 12, SoundId::LeechBurst);
        rest(leech, 90);
        break;
    default: rest(leech, 60); break;
    }
}
