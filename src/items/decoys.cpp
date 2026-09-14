#include "decoys.hpp"
#include "../props/growth.hpp"

namespace {
// WARD: A compact destructible prop, without an actor or behavior allocation.
constexpr RegionalItem scarecrow{"Scarecrow",
    "Birds and rabbits avoid its ward. Won't stop a committed attack. Burns and breaks.",
    Sprite::ScarecrowBundle, {1, 1, 4, 0, 45, PatternEffect::Utility},
    ItemAction::Material, 16, 2, true, 0, 0, 0, 0, 0, SoundId::ScarecrowPlant};
}

const RegionalItem* forest_decoy(ItemKind kind) {
    return kind == ItemKind::Scarecrow ? &scarecrow : nullptr;
}

bool place_scarecrow(Game& game, int slot, Cell direction) {
    const Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const Cell cell = user.cell + direction;
    if (!plant_prop(game, cell, PropKind::Scarecrow)) return false;
    game.stage.at(cell)->prop.variant = static_cast<std::uint8_t>(item_pattern(*user.inventory.held()).blast_radius);
    return true;
}
