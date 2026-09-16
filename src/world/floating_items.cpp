#include "../entities/river_raft.hpp"
#include "../items/sled.hpp"
#include "floating_items.hpp"
#include "currents.hpp"
#include "water.hpp"
#include "../surfaces/interaction.hpp"

bool floating_item(const Entity& item) {
    return item.kind == EntityKind::GroundItem && item.label_a == 1;
}

bool float_water(const Tile& tile) {
    return shallow_water(tile.kind) || (walkable(tile) && tile.kind != TileKind::Water &&
        water_liquid(tile.surface.liquid) && tile.surface.liquid_ticks > 0);
}

bool float_cell_free(const Game& game, Cell cell, int cargo_slot) {
    const Tile* tile = game.stage.at(cell);
    if (tile == nullptr || !walkable(*tile) || tile->kind == TileKind::Lava) return false;
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& other = game.entities[static_cast<std::size_t>(slot)];
        if (slot != cargo_slot && other.kind != EntityKind::None && other.cell == cell &&
            (other.impassable || other.kind == EntityKind::GroundItem)) return false;
    }
    return true;
}

// CARGO SLOTS: GroundItem label_a = floating; counter_a = cells left;
// timer_a = travel beat; point_a = expected cell; point_b = heading.
// The original ground_item, sprite and generation remain the actual cargo.
bool start_item_float(Game& game, int slot, Cell direction) {
    Entity& cargo = game.entities[static_cast<std::size_t>(slot)];
    if (cargo.kind != EntityKind::GroundItem || cargo.ground_item.count <= 0 ||
        cargo.ground_item.kind == ItemKind::None || sled_cargo(game,cargo) || ridden_river_raft(game,cargo) || floating_item(cargo) || distance({}, direction) != 1 ||
        !float_water(game.stage.at_or_border(cargo.cell)) ||
        !float_cell_free(game, cargo.cell, slot) || !float_cell_free(game, cargo.cell + direction, slot)) return false;
    cargo.label_a = 1;
    cargo.counter_a = item_float_reach;
    cargo.timer_a = water_current_beat(game.stage.at_or_border(cargo.cell),item_float_beat);
    cargo.point_a = cargo.cell;
    cargo.point_b = direction;
    emit_sound(game, SoundId::AirFloat, cargo.cell);
    return true;
}

void stop_item_float(Game& game, Entity& cargo) {
    if (!floating_item(cargo)) return;
    cargo.label_a = cargo.counter_a = cargo.timer_a = 0;
    cargo.point_a = cargo.point_b = {};
    emit_sound(game, SoundId::AirDeflate, cargo.cell);
}

void step_floating_item(Game& game, int slot) {
    Entity& cargo = game.entities[static_cast<std::size_t>(slot)];
    if (!floating_item(cargo)) return;
    // DISPLACEMENT: Hooks or shoves take over; drying water also ends this trip.
    if (cargo.cell != cargo.point_a || distance({}, cargo.point_b) != 1 || !float_water(game.stage.at_or_border(cargo.cell)) || cargo.counter_a <= 0) {
        stop_item_float(game, cargo);
        return;
    }
    if (game.stage.at_or_border(cargo.cell).surface.still_ticks>0) return;
    if (cargo.timer_a > 0) return;
    const Cell flow=water_current(game.stage.at_or_border(cargo.cell));
    if (flow!=Cell{}) cargo.point_b=flow;
    const Cell next = cargo.cell + cargo.point_b;
    if (!float_cell_free(game, next, slot)) { stop_item_float(game, cargo); return; }
    cargo.cell = cargo.point_a = next;
    cargo.timer_a = water_current_beat(game.stage.at_or_border(next),item_float_beat);
    --cargo.counter_a;
    // BEACH: One dry landing is allowed; deep water and occupied landings are not.
    if (!float_water(game.stage.at_or_border(next)) || cargo.counter_a == 0)
        stop_item_float(game, cargo);
    else if (cargo.counter_a % 3 == 0) emit_sound(game, SoundId::AirPaddle, next);
}
