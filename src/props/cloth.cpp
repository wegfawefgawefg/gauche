#include "cloth.hpp"
#include "../surfaces/interaction.hpp"

// FIXTURES: Cloth covers an optical face, without making the cell solid to actors.
bool coverable_prop(const Prop& prop) {
    return !prop.broken && prop.hp > 0 && (prop.kind == PropKind::MirrorShard ||
        prop.kind == PropKind::CrystalLens || prop.kind == PropKind::BeamLamp);
}

bool can_cover_optic(const Tile& tile) {
    return coverable_prop(tile.prop) && !tile.prop.covered &&
        tile.surface.fire_ticks == 0 && !surface_wet(tile);
}

bool cover_optic(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (!tile || !can_cover_optic(*tile)) return false;
    tile->prop.covered = true;
    return true;
}

void remove_prop_cover(Game& game, Cell cell, bool burning) {
    Tile* tile = game.stage.at(cell);
    if (!tile || !tile->prop.covered) return;
    tile->prop.covered = false;
    emit_sound(game, burning ? SoundId::FeltBurn : SoundId::FeltTear, cell);
}

// TEARING: Uncovering destroys the cloth; no free item or accidental gun reload.
bool uncover_optic(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    Item& held = *actor.inventory.held();
    if ((held.kind != ItemKind::None && held.kind != ItemKind::BlackFelt) || held.cooldown > 0)
        return false;
    const Cell cell = actor.cell + actor.facing;
    const Tile* tile = game.stage.at(cell);
    if (!tile || !tile->prop.covered || !coverable_prop(tile->prop)) return false;
    remove_prop_cover(game, cell, false);
    held.cooldown = 12;
    return true;
}
