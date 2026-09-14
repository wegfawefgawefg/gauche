#include "temperature.hpp"

namespace {

bool hot_item(const Item& item) {
    return item.kind != ItemKind::None && (item.flame_ticks > 0 || item.kind == ItemKind::Torch);
}

} // namespace

bool hot_cell(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    if (tile == nullptr) return false;
    if (tile->kind == TileKind::Lava || tile->surface.fire_ticks > 0) return true;
    // SOURCES: A lamp's color is not heat. Only exposed flames melt cold projectiles.
    for (const Entity& actor : game.entities) {
        if (actor.kind == EntityKind::None || actor.cell != cell) continue;
        if (actor.kind == EntityKind::GroundItem && hot_item(actor.ground_item)) return true;
        if (actor.health <= 0) continue;
        if ((actor.kind == EntityKind::Campfire && actor.fire_tramples < 5) ||
            actor.kind == EntityKind::Ember || actor.burn_ticks > 0 || actor.scorch_ticks > 0) return true;
        const Item* held = actor.inventory.held();
        if (held && hot_item(*held)) return true;
    }
    return false;
}
