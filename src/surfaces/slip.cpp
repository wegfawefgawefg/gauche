#include "slip.hpp"
#include "../world/water.hpp"

bool bare_ice(const Tile& tile) {
    const bool sticky = tile.surface.liquid_ticks > 0 &&
        (tile.surface.liquid == LiquidKind::Sap || tile.surface.liquid == LiquidKind::Honey ||
         tile.surface.liquid == LiquidKind::SpentSap);
    return tile.kind == TileKind::Ice && !tile.surface.gritted && !sticky;
}

bool slip_on_surface(Game& game, int slot, Cell direction) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    const Tile* tile = game.stage.at(actor.cell);
    if (tile == nullptr) return false;
    const bool oil = tile->surface.liquid == LiquidKind::Oil && tile->surface.liquid_ticks > 0;
    const bool ice = bare_ice(*tile) && actor.kind != EntityKind::RimeSkater;
    if ((!oil && !ice) ||
        !wading_actor(actor) || actor.vitals.grip > 0 || actor.vitals.rooted > 0 || actor.hard_blocker ||
        distance({}, direction) != 1) return false;
    const Cell next = actor.cell + direction;
    tile = game.stage.at(next);
    if (tile == nullptr || !walkable(*tile) || entity_at(game, next, true) >= 0) return false;
    // SLIP: One extra real cell per step; no recursion along a whole lake.
    if (!move_entity(game, slot, next, false)) return false;
    emit_sound(game, oil ? SoundId::OilSlip : SoundId::IceSlip, actor.cell);
    return true;
}
