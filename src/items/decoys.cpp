#include "decoys.hpp"
#include "../props/growth.hpp"

namespace {
// WARD: A compact destructible prop, without an actor or behavior allocation.
constexpr RegionalItem scarecrow{"Scarecrow",
    "Birds and rabbits avoid its ward. Won't stop a committed attack. Burns and breaks.",
    Sprite::ScarecrowBundle, {1, 1, 4, 0, 45, PatternEffect::Utility},
    ItemAction::Material, 16, 2, true, 0, 0, 0, 0, 0, SoundId::ScarecrowPlant};
constexpr RegionalItem straw{"Straw Decoy",
    "Draws enemy attacks. A straw body that tears and burns. Some creatures ignore it.",
    Sprite::StrawDecoyBundle, {1, 1, 6, 0, 45, PatternEffect::Utility},
    ItemAction::Material, 12, 2, true, 0, 0, 0, 0, 0, SoundId::DecoyPlant};
}

const RegionalItem* forest_decoy(ItemKind kind) {
    if (kind == ItemKind::StrawDecoy) return &straw;
    return kind == ItemKind::Scarecrow ? &scarecrow : nullptr;
}

bool place_decoy(Game& game, int slot, Cell direction) {
    const Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const Cell cell = user.cell + direction;
    const PropKind kind = user.inventory.held()->kind == ItemKind::StrawDecoy ? PropKind::StrawDecoy : PropKind::Scarecrow;
    if (!plant_prop(game, cell, kind)) return false;
    game.stage.at(cell)->prop.variant = static_cast<std::uint8_t>(item_pattern(*user.inventory.held()).blast_radius);
    return true;
}
