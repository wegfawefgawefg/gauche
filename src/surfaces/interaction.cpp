#include "../scenery/roof.hpp"
#include "../entities/slag_snail.hpp"
#include "slag.hpp"
#include "../items/emergency_foam.hpp"
#include "../entities/pressure_rat.hpp"
#include "../entities/ember.hpp"
#include "interaction.hpp"
#include "temperature.hpp"
#include "../entities/icicle_spider.hpp"
#include "../props/candle.hpp"
#include "../props/stove.hpp"
#include "../world/terrain_material.hpp"
#include "../world/water.hpp"
#include "../props/interaction.hpp"
#include "../props/cloth.hpp"

#include <algorithm>
#include <cstdlib>
#include <vector>

namespace {

bool dry_growth(const Prop& prop) {
    if (prop.broken || prop.kind == PropKind::None) return false;
    if (prop.kind==PropKind::FoamCover) return false;
    if (prop.covered) return true;
    if (prop.kind==PropKind::IcePillar || prop.kind==PropKind::IceRubble) return false;
    if (prop.kind == PropKind::ChapelUrn || prop.kind == PropKind::ChapelWax) return false;
    return prop.kind != PropKind::HoistWreck && prop.kind != PropKind::PayCage && prop.kind != PropKind::TensionSpring && prop.kind != PropKind::Barricade && prop.kind != PropKind::Conveyor && prop.kind != PropKind::Grate && prop.kind != PropKind::ScrapBin && prop.kind != PropKind::OreBin && prop.kind != PropKind::CrystalGrowth && prop.kind != PropKind::Doorstop && prop.kind != PropKind::CopperWire && prop.kind != PropKind::GroundingSpike && prop.kind != PropKind::MaintenanceLocker && prop.kind != PropKind::Stove && prop.kind != PropKind::Candle && prop.kind != PropKind::FrozenLunchTin && prop.kind != PropKind::WeatherVane && prop.kind != PropKind::AlarmClock && prop.kind != PropKind::BeamLamp && prop.kind != PropKind::MirrorShard && prop.kind != PropKind::CrystalLens && prop.kind != PropKind::SnowCache && prop.kind != PropKind::ClayPot && prop.kind != PropKind::IceBlock;
}

} // namespace

bool surface_wet(const Tile& tile) {
    return shallow_water(tile.kind) || tile.kind == TileKind::Water ||
        (water_liquid(tile.surface.liquid) && tile.surface.liquid_ticks > 0);
}

bool pour_surface(Game& game, Cell cell, LiquidKind kind, int ticks) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr || tile->kind == TileKind::Wall || !walkable(tile->kind)) return false;
    Surface& surface = tile->surface;
    if (kind == LiquidKind::Rot && surface.fire_ticks > 0) return false;
    if (water_liquid(kind)) {
        // Water reaches actors and fixtures as well as the floor beneath them.
        quench_cell(game, cell, SoundId::WaterDouse);
        surface.gritted = false;
        surface.smoke_ticks = static_cast<std::uint16_t>(std::min(45, static_cast<int>(surface.smoke_ticks)));
        surface.sleep_ticks = 0;
        surface.scent_ticks = 0;
        if (tile->kind == TileKind::Lava) *tile = {TileKind::Ruin, 0, 0};
    } else if (surface_wet(*tile)) return false;
    surface.liquid = kind;
    surface.liquid_ticks = static_cast<std::uint16_t>(std::clamp(ticks, 0, 3600));
    if (kind==LiquidKind::Tar && ticks>0) {
        if (hot_cell(game,cell)) ignite_surface(game,cell);
        else if (tile->kind==TileKind::Ice) {
            surface.liquid=LiquidKind::ClottedTar;emit_sound(game,SoundId::TarClot,cell);
        }
    }
    return true;
}

bool ignite_surface(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr || surface_wet(*tile) || live_foam(tile->prop)) return false;
    if (tile->prop.kind == PropKind::SpiderStrand) return cut_spider_strand(game,cell,true);
    const bool wood = tile->kind == TileKind::Wall && wooden_terrain(*tile) &&
        tile->break_rule != BreakRule::Unbreakable;
    if (tile->kind == TileKind::Wall && !wood) return false;
    Surface& surface = tile->surface;
    const bool lit = light_candle(game,cell) || light_stove(game,cell);
    if (surface.fire_ticks > 0) return lit;
    const bool fuel = (surface.liquid == LiquidKind::Oil || surface.liquid == LiquidKind::Sap || tar_liquid(surface.liquid)) && surface.liquid_ticks>0;
    if (!wood && !wooden_roof(game.stage,cell) && !fuel && !dry_growth(tile->prop)) return lit;
    const bool cloth_only = tile->prop.covered && !wood && !fuel;
    remove_prop_cover(game, cell, true);
    surface.fire_ticks = static_cast<std::uint16_t>(cloth_only ? 30 : wood ? 600 : surface.liquid == LiquidKind::Sap ? 360 : 240);
    surface.smoke_ticks = std::max<std::uint16_t>(surface.smoke_ticks, 100);
    if (surface.liquid == LiquidKind::Oil) surface.liquid_ticks = 240;
    if (surface.liquid == LiquidKind::Sap) surface.liquid_ticks = 360;
    if (tar_liquid(surface.liquid)) {
        surface.liquid=LiquidKind::Tar;
        surface.fire_ticks=surface.liquid_ticks=std::min<std::uint16_t>(240,surface.liquid_ticks);
    }
    emit_sound(game, SoundId::FireCatch, cell);
    return true;
}

void contact_surface(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    if (actor.toss.ticks>0 || actor.health <= 0 || (actor.move_interval <= 0 && actor.kind != EntityKind::RootTurret && actor.kind != EntityKind::WaspNest) || actor.kind == EntityKind::Train) return;
    Tile* tile = game.stage.at(actor.cell);
    if (tile == nullptr) return;
    if (wading_actor(actor) || actor.kind == EntityKind::RootTurret || actor.kind == EntityKind::WaspNest) {
        if (surface_wet(*tile)) {
            damp_stoker(actor);cool_pressure_rat(actor);cool_slag_snail(actor);
            if (actor.burn_ticks > 0 || actor.scorch_ticks > 0) emit_sound(game, SoundId::WaterDouse, actor.cell);
            actor.burn_ticks = actor.scorch_ticks = 0;
            actor.vitals.nausea = actor.vitals.nausea_wait = 0;
        } else {
            if (tile->surface.liquid == LiquidKind::Rot && tile->surface.liquid_ticks > 0) {
                if (actor.vitals.nausea == 0 && actor.kind == EntityKind::Player) emit_sound(game, SoundId::NauseaGag, actor.cell);
                apply_nausea(actor, 180);
            }
            if (actor.burn_ticks > 0 || actor.scorch_ticks > 0) ignite_surface(game, actor.cell);
            if (tile->surface.fire_ticks > 0 && actor.kind != EntityKind::SlagSnail && actor.kind != EntityKind::SteamLeech && actor.kind != EntityKind::WalkingKiln && actor.kind != EntityKind::Ember) {
                if (actor.scorch_ticks == 0) emit_sound(game, SoundId::FirePanic, actor.cell);
                actor.scorch_ticks = 300;
            }
        }
    }
    crack_slag(game,slot);
    if (tile->surface.sleep_ticks > 0 && game.tick % 30 == 0 && actor.kind != EntityKind::Ember)
        apply_sleep(actor, 90);
}

int surface_step_delay(const Tile& tile) {
    if (tile.surface.liquid_ticks==0) return 0;
    if (tile.surface.liquid==LiquidKind::Tar) return 12;
    return tile.surface.liquid == LiquidKind::Sap || tile.surface.liquid == LiquidKind::Honey || tile.surface.liquid == LiquidKind::SpentSap ? 8 : 0;
}

void step_surfaces(Game& game) {
    std::vector<Cell> spread;
    for (int y = 0; y < game.stage.height; ++y)
        for (int x = 0; x < game.stage.width; ++x) {
            const Cell cell{x, y};
            Tile& tile = *game.stage.at(cell);
            Surface& surface = tile.surface;
            if (surface_wet(tile)) surface.gritted = false;
            if (surface.liquid_ticks > 0 && --surface.liquid_ticks == 0) surface.liquid = LiquidKind::None;
            if (surface.smoke_ticks > 0) --surface.smoke_ticks;
            if (surface.whiteout_ticks > 0) --surface.whiteout_ticks;
            if (surface.still_ticks > 0) --surface.still_ticks;
            if (surface.sleep_ticks > 0) --surface.sleep_ticks;
            if (surface.scent_ticks > 0) --surface.scent_ticks;
            if (surface_wet(tile) || surface.fire_ticks > 0) surface.scent_ticks = 0;
            if (surface.fire_ticks > 0 && surface.liquid == LiquidKind::Rot) {
                surface.liquid = LiquidKind::None; surface.liquid_ticks = 0;
            }
            if (surface.fire_ticks == 0) {surface.reactor_fire=false;continue;}
            if (surface_wet(tile)) { surface.fire_ticks = 0; surface.reactor_fire=false; continue; }
            remove_prop_cover(game, cell, true);
            --surface.fire_ticks;
            if (surface.fire_ticks==0) surface.reactor_fire=false;
            if (surface.fire_ticks == 0 && surface.liquid == LiquidKind::Sap && surface.liquid_ticks > 0)
                surface.liquid = LiquidKind::SpentSap;
            if (surface.fire_ticks == 0 || game.tick % 30 != 0) continue;
            surface.smoke_ticks = 100;
            hit_prop(game, cell, 5, cell);
            if (tile.kind == TileKind::Wall && wooden_terrain(tile)) hit_terrain(game, cell, cell, 6, 0);
            for (Cell side : {Cell{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) spread.push_back(cell + side);
        }
    // PROPAGATION: Newly ignited neighbors cannot cascade across a floor in this tick.
    for (Cell cell : spread) ignite_surface(game, cell);
}

bool smoke_hides(const Stage& stage, Cell from, Cell to) {
    const int steps = std::max(std::abs(to.x - from.x), std::abs(to.y - from.y));
    if (steps == 0) return obscures_sight(stage.at_or_border(from).surface);
    for (int step = 0; step <= steps; ++step) {
        const Cell cell{from.x + (to.x - from.x) * step / steps,
                        from.y + (to.y - from.y) * step / steps};
        if (obscures_sight(stage.at_or_border(cell).surface)) return true;
    }
    return false;
}
