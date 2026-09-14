#include "temperature.hpp"
#include "../world/water.hpp"

#include <algorithm>
#include <vector>

namespace {

bool hot_item(const Item& item) {
    return item.kind != ItemKind::None && (item.flame_ticks > 0 || item.kind == ItemKind::Torch);
}

bool hot_actor(const Entity& actor) {
    if (actor.kind == EntityKind::None) return false;
    if (actor.kind == EntityKind::GroundItem) return hot_item(actor.ground_item);
    if (actor.health <= 0) return false;
    if ((actor.kind == EntityKind::Campfire && actor.fire_tramples < 5) ||
        actor.kind == EntityKind::Ember || actor.burn_ticks > 0 || actor.scorch_ticks > 0) return true;
    const Item* held = actor.inventory.held();
    return held && hot_item(*held);
}

} // namespace

bool hot_cell(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    if (tile == nullptr) return false;
    if (tile->kind == TileKind::Lava || tile->surface.fire_ticks > 0) return true;
    // SOURCES: A lamp's color is not heat. Only exposed flames melt cold projectiles.
    for (const Entity& actor : game.entities) {
        if (actor.cell == cell && hot_actor(actor)) return true;
    }
    return false;
}

bool warm_cell(const Game& game, Cell cell) {
    for (Cell offset : {Cell{0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}})
        if (hot_cell(game, cell + offset)) return true;
    return false;
}

bool freeze_water(Game& game, Cell cell, int ticks) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr || ticks <= 0 || warm_cell(game, cell)) return false;
    // MEMORY: Refresh temporary ice without forgetting the pool or diver hole beneath it.
    if (tile->kind != TileKind::Ice || tile->freeze_ticks == 0) {
        if (!shallow_water(tile->kind)) return false;
        tile->thaw_kind = tile->kind;
    }
    tile->kind = TileKind::Ice;
    tile->freeze_ticks = static_cast<std::uint16_t>(std::clamp(
        std::max(ticks, static_cast<int>(tile->freeze_ticks)), 1, 480));
    if (tile->surface.liquid == LiquidKind::Water) {
        tile->surface.liquid = LiquidKind::None;
        tile->surface.liquid_ticks = 0;
    }
    tile->surface.gritted = false;
    return true;
}

bool thaw_water(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr || tile->kind != TileKind::Ice) return false;
    // BANKS: Native ice melts to shallow water; a temporary lid restores its exact source.
    tile->kind = tile->freeze_ticks > 0 ? tile->thaw_kind : TileKind::ShallowWater;
    tile->thaw_kind = TileKind::Empty;
    tile->freeze_ticks = 0;
    tile->surface.gritted = false;
    return true;
}

void quench_cell(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr) return;
    bool quenched = tile->surface.fire_ticks > 0;
    tile->surface.fire_ticks = 0;
    for (Entity& actor : game.entities) {
        if (actor.kind == EntityKind::None || actor.cell != cell) continue;
        quenched |= actor.burn_ticks > 0 || actor.scorch_ticks > 0;
        actor.burn_ticks = actor.scorch_ticks = 0;
        if (actor.kind == EntityKind::Campfire && actor.fire_tramples < 5) {
            quenched = true;
            actor.fire_tramples = 5;
            actor.fire_dim_ticks = 0;
            actor.sprite = Sprite::CampfireAsh;
            actor.light = {};
            actor.self_light = {};
        }
        // FUEL: Temporary burning sticks go out. Torches and living embers retain their flame.
        Item* exposed = actor.kind == EntityKind::GroundItem ? &actor.ground_item : actor.inventory.held();
        if (exposed && exposed->flame_ticks > 0) {
            exposed->flame_ticks = 0;
            quenched = true;
        }
    }
    if (quenched) emit_sound(game, SoundId::ColdQuench, cell);
}

void step_temperature(Game& game) {
    std::vector<Cell> flames;
    // TERRAIN: No per-tile scan through every actor. Collect actual heat sources once.
    for (int y = 0; y < game.stage.height; ++y)
        for (int x = 0; x < game.stage.width; ++x) {
            const Cell cell{x, y};
            Tile& tile = *game.stage.at(cell);
            if (tile.freeze_ticks > 0) {
                if (tile.kind != TileKind::Ice) {
                    tile.freeze_ticks = 0; tile.thaw_kind = TileKind::Empty;
                } else if (tile.freeze_ticks == 1) {
                    thaw_water(game, cell);
                    emit_sound(game, SoundId::IceThaw, cell);
                } else --tile.freeze_ticks;
            }
            if (tile.kind == TileKind::Lava || tile.surface.fire_ticks > 0) flames.push_back(cell);
        }
    for (const Entity& actor : game.entities)
        if (hot_actor(actor)) flames.push_back(actor.cell);
    for (Cell flame : flames) {
        bool thawed = false;
        for (Cell offset : {Cell{0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}})
            thawed |= thaw_water(game, flame + offset);
        if (thawed) emit_sound(game, SoundId::IceThaw, flame);
    }
}
