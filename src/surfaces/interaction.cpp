#include "interaction.hpp"
#include "../world/water.hpp"
#include "../props/interaction.hpp"

#include <algorithm>
#include <cstdlib>
#include <vector>

namespace {

bool dry_growth(const Prop& prop) {
    if (prop.broken || prop.kind == PropKind::None) return false;
    return prop.kind != PropKind::ClayPot;
}

} // namespace

bool surface_wet(const Tile& tile) {
    return shallow_water(tile.kind) || tile.kind == TileKind::Water ||
        (tile.surface.liquid == LiquidKind::Water && tile.surface.liquid_ticks > 0);
}

bool pour_surface(Game& game, Cell cell, LiquidKind kind, int ticks) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr || tile->kind == TileKind::Wall || !walkable(tile->kind)) return false;
    Surface& surface = tile->surface;
    if (kind == LiquidKind::Water) {
        if (surface.fire_ticks > 0) emit_sound(game, SoundId::WaterDouse, cell);
        surface.fire_ticks = 0;
        surface.smoke_ticks = static_cast<std::uint16_t>(std::min(45, static_cast<int>(surface.smoke_ticks)));
        surface.sleep_ticks = 0;
        if (tile->kind == TileKind::Lava) *tile = {TileKind::Ruin, 0, 0};
    } else if (surface_wet(*tile)) return false;
    surface.liquid = kind;
    surface.liquid_ticks = static_cast<std::uint16_t>(std::clamp(ticks, 0, 3600));
    return true;
}

bool ignite_surface(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr || tile->kind == TileKind::Wall || surface_wet(*tile)) return false;
    Surface& surface = tile->surface;
    if (surface.fire_ticks > 0) return false;
    const bool fuel = surface.liquid == LiquidKind::Oil || surface.liquid == LiquidKind::Sap;
    if (!fuel && !dry_growth(tile->prop)) return false;
    surface.fire_ticks = static_cast<std::uint16_t>(surface.liquid == LiquidKind::Sap ? 360 : 240);
    surface.smoke_ticks = std::max<std::uint16_t>(surface.smoke_ticks, 100);
    if (surface.liquid == LiquidKind::Oil) surface.liquid_ticks = 240;
    if (surface.liquid == LiquidKind::Sap) surface.liquid_ticks = 360;
    emit_sound(game, SoundId::FireCatch, cell);
    return true;
}

void contact_surface(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    if (actor.health <= 0 || (actor.move_interval <= 0 && actor.kind != EntityKind::RootTurret) || actor.kind == EntityKind::Train) return;
    Tile* tile = game.stage.at(actor.cell);
    if (tile == nullptr) return;
    if (wading_actor(actor) || actor.kind == EntityKind::RootTurret) {
        if (surface_wet(*tile)) {
            if (actor.burn_ticks > 0 || actor.scorch_ticks > 0) emit_sound(game, SoundId::WaterDouse, actor.cell);
            actor.burn_ticks = actor.scorch_ticks = 0;
        } else {
            if (actor.burn_ticks > 0 || actor.scorch_ticks > 0) ignite_surface(game, actor.cell);
            if (tile->surface.fire_ticks > 0) {
                if (actor.scorch_ticks == 0) emit_sound(game, SoundId::FirePanic, actor.cell);
                actor.scorch_ticks = 300;
            }
        }
    }
    if (tile->surface.sleep_ticks > 0 && game.tick % 30 == 0 && actor.kind != EntityKind::Ember)
        actor.sleep_ticks = std::max(actor.sleep_ticks, 90);
}

int surface_step_delay(const Tile& tile) {
    return tile.surface.liquid == LiquidKind::Sap || tile.surface.liquid == LiquidKind::Honey ? 8 : 0;
}

void step_surfaces(Game& game) {
    std::vector<Cell> spread;
    for (int y = 0; y < game.stage.height; ++y)
        for (int x = 0; x < game.stage.width; ++x) {
            const Cell cell{x, y};
            Tile& tile = *game.stage.at(cell);
            Surface& surface = tile.surface;
            if (surface.liquid_ticks > 0 && --surface.liquid_ticks == 0) surface.liquid = LiquidKind::None;
            if (surface.smoke_ticks > 0) --surface.smoke_ticks;
            if (surface.sleep_ticks > 0) --surface.sleep_ticks;
            if (surface.fire_ticks == 0) continue;
            if (surface_wet(tile)) { surface.fire_ticks = 0; continue; }
            --surface.fire_ticks;
            if (surface.fire_ticks == 0 || game.tick % 30 != 0) continue;
            surface.smoke_ticks = 100;
            hit_prop(game, cell, 5, cell);
            for (Cell side : {Cell{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) spread.push_back(cell + side);
        }
    // PROPAGATION: Newly ignited neighbors cannot cascade across a floor in this tick.
    for (Cell cell : spread) ignite_surface(game, cell);
}

bool smoke_hides(const Stage& stage, Cell from, Cell to) {
    const int steps = std::max(std::abs(to.x - from.x), std::abs(to.y - from.y));
    if (steps == 0) return stage.at_or_border(from).surface.smoke_ticks >= 60;
    for (int step = 0; step <= steps; ++step) {
        const Cell cell{from.x + (to.x - from.x) * step / steps,
                        from.y + (to.y - from.y) * step / steps};
        if (stage.at_or_border(cell).surface.smoke_ticks >= 60) return true;
    }
    return false;
}
