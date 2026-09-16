#include "../world/fissures.hpp"
#include "../world/ice_material.hpp"
#include "../items/glow_slag.hpp"
#include "../entities/ash_sleeper.hpp"
#include "../entities/slag_snail.hpp"
#include "../entities/furnace_moth.hpp"
#include "../entities/walking_kiln.hpp"
#include "../entities/pressure_rat.hpp"
#include "../items/flare.hpp"
#include "temperature.hpp"
#include "../projectiles/exposed_fuse.hpp"
#include "../entities/ember.hpp"
#include "../projectiles/coal_spit.hpp"
#include "../projectiles/arrow_fire.hpp"
#include "../props/circuits.hpp"
#include "../props/candle.hpp"
#include "../props/stove.hpp"
#include "../props/lunch_tin.hpp"
#include "../world/snow.hpp"
#include "../props/ice_cover.hpp"
#include "interaction.hpp"
#include "../entities/steam_leech.hpp"
#include "../entities/candle_keeper.hpp"
#include "../world/water.hpp"

#include <algorithm>
#include <vector>

namespace {

bool hot_item(const Item& item) {
    return item.kind != ItemKind::None && (item.flame_ticks > 0 || item.kind == ItemKind::Torch || glowing_slag(item));
}

} // namespace

bool entity_has_flame(const Entity& actor) {
    if (actor.kind == EntityKind::None || actor.kind == EntityKind::SteamLeech) return false;
    if (actor.kind == EntityKind::GroundItem) return hot_item(actor.ground_item);
    if (actor.kind==EntityKind::Projectile && glowing_slag(actor.ground_item)) return true;
    if (burning_arrow(actor)) return true;
    if (actor.health <= 0) return false;
    if (burning_flare(actor)) return true;
    if (actor.kind == EntityKind::CandleKeeper && actor.timer_b == 0) return true;
    if ((actor.kind == EntityKind::Campfire && actor.fire_tramples < 5) ||
        kiln_hot(actor) || stoker_hot(actor) || actor.burn_ticks > 0 || actor.scorch_ticks > 0) return true;
    const Item* held = actor.inventory.held();
    return held && hot_item(*held);
}

bool flame_cell(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    if (tile == nullptr) return false;
    if (prop_has_flame(tile->prop) || tile->kind == TileKind::Lava || tile->surface.fire_ticks > 0) return !leech_drains_cell(game, cell);
    if (fissure_flame(game,cell)) return !leech_drains_cell(game,cell);
    // SOURCES: A lamp's color is not heat. Only exposed flames melt cold projectiles.
    for (const Entity& actor : game.entities) {
        if (actor.cell == cell && entity_has_flame(actor)) return !leech_drains_cell(game, cell);
    }
    return false;
}

bool hot_cell(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    return tile && (((tile->surface.warmth_ticks > 0 || fissure_hot(game,cell)) && !leech_drains_cell(game, cell)) || flame_cell(game, cell));
}

bool warm_cell(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    if (tile && tile->surface.warmth_ticks > 0 && !leech_drains_cell(game, cell)) return true;
    // REACH: A capsule already paints its area. Flames and pressurized fissures warm adjacent cells.
    for (Cell offset : {Cell{0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}})
        if (flame_cell(game, cell + offset) || (fissure_hot(game,cell+offset) && !leech_drains_cell(game,cell+offset))) return true;
    return false;
}

bool warm_surface(Game& game, Cell cell, int ticks) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr || !walkable(tile->kind) || ticks <= 0) return false;
    tile->surface.warmth_ticks = static_cast<std::uint16_t>(std::clamp(
        std::max(ticks, static_cast<int>(tile->surface.warmth_ticks)), 1, 240));
    if (leech_drains_cell(game, cell)) return true;
    if (tile->surface.liquid==LiquidKind::ClottedTar && tile->surface.liquid_ticks>0) {
        tile->surface.liquid=LiquidKind::Tar;emit_sound(game,SoundId::TarSoften,cell);
    }
    clear_snow(game, cell);
    melt_ice_cover(game, cell);
    thaw_lunch_tin(game, cell);
    if (thaw_water(game, cell)) emit_sound(game, SoundId::IceThaw, cell);
    ignite_surface(game, cell);
    for (Entity& actor : game.entities)
        if (actor.kind != EntityKind::None && actor.cell == cell) actor.freeze_ticks = 0;
    return true;
}

bool freeze_water(Game& game, Cell cell, int ticks) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr || ticks <= 0 || warm_cell(game, cell)) return false;
    if ((tile->surface.liquid == LiquidKind::Brine || tile->surface.liquid == LiquidKind::Coolant) && tile->surface.liquid_ticks > 0) return false;
    if (tile->surface.liquid==LiquidKind::Tar && tile->surface.liquid_ticks>0) {
        tile->surface.liquid=LiquidKind::ClottedTar;emit_sound(game,SoundId::TarClot,cell);
        return true; // Cooling this material hardens it without creating a water tile.
    }
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

void quench_cell(Game& game, Cell cell, SoundId sound) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr) return;
    bool quenched = cool_fissure(game,cell) || tile->surface.fire_ticks > 0;
    cool_grounding_spike(game,cell);
    douse_candle(game,cell);
    douse_stove(game,cell);
    tile->surface.fire_ticks = 0;
    tile->surface.reactor_fire=false;
    for (Entity& actor : game.entities) {
        if (actor.kind == EntityKind::None || actor.cell != cell) continue;
        if (quench_exposed_fuse(actor,sound==SoundId::ColdQuench)) quenched=true;
        if (damp_ash_sleeper(actor)) {quenched=true;emit_sound(game,SoundId::AshSettle,cell);}
        if (cool_slag_snail(actor)) {quenched=true;emit_sound(game,SoundId::SlagCool,cell);}
        if (cool_furnace_moth(actor)) quenched=true;
        if (cool_walking_kiln(actor)) quenched=true;
        if (cool_pressure_rat(actor)) quenched=true;
        if (damp_stoker(actor) || douse_coal_spit(actor)) quenched = true;
        if (douse_arrow(actor)) quenched=true;
        if (douse_flare(game,actor)) quenched = true;
        if (douse_keeper_lamp(game,actor)) quenched = true;
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
        Item* slag=actor.kind==EntityKind::Projectile ? &actor.ground_item : exposed;
        if (slag && glowing_slag(*slag)) {slag->loaded=0;quenched=true;emit_sound(game,SoundId::GlowCool,cell);}
        if (exposed && exposed->flame_ticks > 0) {
            exposed->flame_ticks = 0;
            quenched = true;
        }
    }
    if (quenched) emit_sound(game, sound, cell);
}

void step_temperature(Game& game) {
    std::vector<Cell> flames;
    const auto drains = leech_drain_cells(game);
    const auto drained = [&drains](Cell cell) {
        return std::find(drains.begin(), drains.end(), cell) != drains.end();
    };
    // TERRAIN: No per-tile scan through every actor. Collect actual heat sources once.
    for (int y = 0; y < game.stage.height; ++y)
        for (int x = 0; x < game.stage.width; ++x) {
            const Cell cell{x, y};
            Tile& tile = *game.stage.at(cell);
            if (tile.surface.warmth_ticks > 0) {
                --tile.surface.warmth_ticks;
                if (tile.surface.warmth_ticks > 0 && !drained(cell)) {
                    clear_snow(game, cell);
                    melt_ice_cover(game, cell);
                    thaw_lunch_tin(game, cell);
                    if (thaw_water(game, cell)) emit_sound(game, SoundId::IceThaw, cell);
                    ignite_surface(game, cell);
                }
            }
            if (tile.freeze_ticks > 0) {
                if (tile.kind != TileKind::Ice) {
                    tile.freeze_ticks = 0; tile.thaw_kind = TileKind::Empty;
                } else if (tile.freeze_ticks == 1) {
                    thaw_water(game, cell);
                    emit_sound(game, SoundId::IceThaw, cell);
                } else --tile.freeze_ticks;
            }
            if ((prop_has_flame(tile.prop) || tile.kind == TileKind::Lava || tile.surface.fire_ticks > 0) && !drained(cell)) flames.push_back(cell);
        }
    for (const Entity& actor : game.entities)
        if (entity_has_flame(actor) && !drained(actor.cell)) flames.push_back(actor.cell);
    for (const auto& f:game.fissures) for (int n=-1;n<=1;++n) {
        const Cell cell=fissure_cell(f,n);if (fissure_flame(game,cell) && !drained(cell)) flames.push_back(cell);
    }
    for (Cell flame : flames) {
        bool thawed = false;
        for (Cell offset : {Cell{0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
            // WICKS: Exposed flames reach nearby candles; colored lights do not.
            if (!drained(flame + offset)) light_candle(game, flame + offset);
            clear_snow(game, flame + offset);
            melt_ice_cover(game, flame + offset);
            thaw_lunch_tin(game, flame + offset);
            thawed |= thaw_water(game, flame + offset);
            if (game.tick%30==0 && !drained(flame+offset))
                melt_ice_wall(game,flame+offset,4,flame);
        }
        if (thawed) emit_sound(game, SoundId::IceThaw, flame);
    }
}
