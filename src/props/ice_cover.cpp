#include "ice_cover.hpp"
#include "interaction.hpp"
#include "../surfaces/interaction.hpp"
#include "../surfaces/temperature.hpp"

bool ice_cover_space(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    if (!tile || !walkable(*tile) || tile->kind == TileKind::Rail || tile->kind == TileKind::Lava ||
        surface_wet(*tile) || tile->surface.fire_ticks > 0 || entity_at(game, cell, false) >= 0 ||
        (tile->prop.kind != PropKind::None && !tile->prop.broken)) return false;
    return !warm_cell(game, cell);
}

bool place_ice_cover(Game& game, Cell cell) {
    if (!ice_cover_space(game, cell)) return false;
    Tile& tile = *game.stage.at(cell);
    tile.prop = {};
    if (!place_prop(game.stage, cell, PropKind::IceBlock)) return false;
    // LIFETIME: Use the existing prop clock. A blocked route always reopens in ten seconds.
    tile.prop.growth_ticks = 600;
    emit_sound(game, SoundId::MasonPlace, cell);
    return true;
}

bool melt_ice_cover(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (!tile || tile->prop.kind != PropKind::IceBlock || tile->prop.broken) return false;
    // MELT: Water, not a shatter event. Leave the underlying floor and other liquids intact.
    tile->prop = {};
    if (tile->surface.liquid == LiquidKind::None || tile->surface.liquid == LiquidKind::Water)
        pour_surface(game, cell, LiquidKind::Water, 180);
    emit_sound(game, SoundId::IceThaw, cell);
    return true;
}

void age_ice_cover(Game& game, Cell cell) {
    Prop& prop = game.stage.at(cell)->prop;
    if (prop.broken) return;
    if (prop.growth_ticks > 0) --prop.growth_ticks;
    if (prop.growth_ticks == 0) melt_ice_cover(game, cell);
}
