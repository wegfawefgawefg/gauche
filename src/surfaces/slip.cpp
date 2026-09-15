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
    if (tile == nullptr || tile->surface.still_ticks>0) return false;
    const bool coolant=tile->surface.liquid==LiquidKind::Coolant && tile->surface.liquid_ticks>0;
    const bool oil = tile->surface.liquid == LiquidKind::Oil && tile->surface.liquid_ticks > 0;
    const bool ice = bare_ice(*tile) && actor.kind != EntityKind::RimeSkater && actor.vitals.traction == 0;
    if ((!oil && !ice && !coolant) ||
        !wading_actor(actor) || actor.vitals.grip > 0 || actor.vitals.rooted > 0 || actor.hard_blocker ||
        distance({}, direction) != 1) return false;
    const Cell next = actor.cell + direction;
    tile = game.stage.at(next);
    if (tile == nullptr || !walkable(*tile) || entity_at(game, next, true) >= 0) return false;
    // SLIP: One extra real cell per step; no recursion along a whole lake.
    if (!move_entity(game, slot, next, false)) return false;
    if (actor.health > 0) actor.vitals.slide_momentum = 12;
    emit_sound(game, coolant ? SoundId::CoolantSlip : oil ? SoundId::OilSlip : SoundId::IceSlip, actor.cell);
    return true;
}
