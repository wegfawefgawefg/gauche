#include "heat_capsule.hpp"
#include "../surfaces/temperature.hpp"
#include "../entities/attacks.hpp"

namespace {

constexpr RegionalItem capsule{"Heat Capsule", "Warm a cross for 4s. Thaw ice, clear chill and resist cold. Ignites spilled oil; supplies no healing.",
    Sprite::HeatCapsule, {0, 0, 1, 0, 45, PatternEffect::Utility, false, 0, 0, false, false, true},
    ItemAction::Material, 8, 4, true, 0, 0, 0, 0, 0, SoundId::HeatCrack};

} // namespace

const RegionalItem* heat_capsule_item(ItemKind kind) {
    return kind == ItemKind::HeatCapsule ? &capsule : nullptr;
}

std::vector<Cell> heat_capsule_cells(const Game& game, const Item& item, Cell center) {
    std::vector<Cell> cells;
    const Tile* tile = game.stage.at(center);
    if (tile == nullptr || !walkable(tile->kind)) return cells;
    cells.push_back(center);
    const int radius = item_pattern(item).blast_radius;
    for (Cell direction : {Cell{1, 0}, {-1, 0}, {0, 1}, {0, -1}})
        for (int reach = 1; reach <= radius; ++reach) {
            const Cell cell = center + Cell{direction.x * reach, direction.y * reach};
            if (!clear_attack_sight(game, center, cell, false)) break;
            cells.push_back(cell);
        }
    return cells;
}

bool use_heat_capsule(Game& game, int slot) {
    const Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    const auto cells = heat_capsule_cells(game, *actor.inventory.held(), actor.cell);
    // REACTION: Later oil spills still ignite while the patch is warm. Water can sit warm.
    bool used = false;
    for (Cell cell : cells) used |= warm_surface(game, cell, 240);
    if (used) emit_sound(game, SoundId::HeatHiss, actor.cell);
    return used;
}
