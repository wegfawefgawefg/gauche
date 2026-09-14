#include "slip.hpp"
#include "../world/water.hpp"

bool slip_on_oil(Game& game, int slot, Cell direction) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    const Tile* tile = game.stage.at(actor.cell);
    if (tile == nullptr || tile->surface.liquid != LiquidKind::Oil || tile->surface.liquid_ticks == 0 ||
        !wading_actor(actor) || actor.vitals.grip > 0 || actor.vitals.rooted > 0 || actor.hard_blocker ||
        distance({}, direction) != 1) return false;
    const Cell next = actor.cell + direction;
    tile = game.stage.at(next);
    if (tile == nullptr || !walkable(*tile) || entity_at(game, next, true) >= 0) return false;
    // SLIP: One extra cell per deliberate step; no recursion along a whole oil lake.
    if (!move_entity(game, slot, next, false)) return false;
    emit_sound(game, SoundId::OilSlip, actor.cell);
    return true;
}
