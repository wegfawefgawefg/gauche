#include "ice_footing.hpp"
#include "../entities/attacks.hpp"
#include "../surfaces/interaction.hpp"

namespace {

constexpr RegionalItem grit{"Grit Pouch", "Scatter traction over ice. Stops slips and brakes skaters. Water washes it away; oil still slips.",
    Sprite::GritPouch, {1, 1, 0, 0, 30, PatternEffect::Utility, false, 1},
    ItemAction::Material, 6, 1, false, 6, 0, 0, 0, 0, SoundId::GritScatter};

} // namespace

const RegionalItem* ice_footing_item(ItemKind kind) {
    return kind == ItemKind::GritPouch ? &grit : nullptr;
}

bool scatter_grit(Game& game, int slot, Cell direction) {
    const Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const ItemPattern pattern = item_pattern(*user.inventory.held());
    const Cell side{-direction.y, direction.x};
    bool used = false;
    for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane)
        for (int reach = pattern.minimum; reach <= pattern.maximum; ++reach) {
            const Cell cell = user.cell + Cell{direction.x * reach + side.x * lane,
                                               direction.y * reach + side.y * lane};
            Tile* tile = game.stage.at(cell);
            if (tile == nullptr || !walkable(*tile) || !clear_sight(game, user.cell, cell)) break;
            // DRY: No invisible expense when the patch is already gritted or under water.
            if (tile->kind != TileKind::Ice || surface_wet(*tile) || tile->surface.gritted) continue;
            tile->surface.gritted = true;
            used = true;
        }
    return used;
}
