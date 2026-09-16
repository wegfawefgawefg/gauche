#include "snow.hpp"
#include "../props/interaction.hpp"
#include "../entities/snow_burrower.hpp"
#include "../surfaces/interaction.hpp"

bool burrowable_snow(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    return tile && tile->kind == TileKind::Snow && !prop_blocks(tile->prop) &&
        !surface_wet(*tile) && tile->surface.fire_ticks == 0 && tile->surface.warmth_ticks == 0;
}

bool clear_snow(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (!tile) return false;
    const bool pile=tile->prop.kind==PropKind::SnowPile && !tile->prop.broken;
    if (tile->kind!=TileKind::Snow && !pile) return false;
    if (pile) hit_prop(game,cell,tile->prop.hp,cell);
    // GROUND: Preserve props and liquids. Cleared ground does not regrow its snow cover.
    if (tile->kind==TileKind::Snow) tile->kind = TileKind::Empty;
    if (tile->prop.kind == PropKind::SnowCache && !tile->prop.broken)
        hit_prop(game, cell, 4, cell);
    for (Entity& actor : game.entities)
        if (actor.kind == EntityKind::SnowBurrower && actor.cell == cell) expose_snow_burrower(actor);
    return true;
}
