#include "slag.hpp"
#include "interaction.hpp"
#include "../world/water.hpp"

bool leave_slag(Game& game,Cell cell) {
    Tile* tile=game.stage.at(cell);
    if (!tile || !walkable(tile->kind) || tile->kind==TileKind::Lava || surface_wet(*tile) ||
        (tile->prop.kind!=PropKind::None && !tile->prop.broken) ||
        (tile->surface.liquid!=LiquidKind::None && tile->surface.liquid!=LiquidKind::Slag)) return false;
    // No terrain conversion or fuel erasure. Heat fades after 1.5s; its safe crust
    // lasts four seconds at most and is broken by the next grounded footstep.
    tile->surface.liquid=LiquidKind::Slag;tile->surface.liquid_ticks=240;
    tile->surface.fire_ticks=90;
    return true;
}
void crack_slag(Game& game,int slot) {
    const auto& actor=game.entities[static_cast<std::size_t>(slot)];
    if (!wading_actor(actor) || actor.kind==EntityKind::SlagSnail || actor.toss.ticks>0) return;
    Tile* tile=game.stage.at(actor.cell);
    if (!tile || tile->surface.liquid!=LiquidKind::Slag || !tile->surface.liquid_ticks || tile->surface.fire_ticks) return;
    tile->surface.liquid=LiquidKind::None;tile->surface.liquid_ticks=0;
    emit_sound(game,SoundId::SlagCrack,actor.cell);
}
