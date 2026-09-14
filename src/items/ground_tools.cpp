#include "ground_tools.hpp"
#include "../props/growth.hpp"

#include <algorithm>

namespace {

constexpr RegionalItem seed{"Bird Seed", "12 beakfuls. Draws idle birds and chicken flocks. Does not cancel committed attacks or family defense.",
    Sprite::BirdSeed, {1, 1, 0, 0, 30, PatternEffect::Utility},
    ItemAction::Material, 4, 5, true, 0, 0, 0, 0, 0, SoundId::SeedScatter};
constexpr RegionalItem thorns{"Thorn Caltrops", "3 hits per patch on entry. Hurts grounded friends and foes. Flyers pass over. Cut or burn to clear.",
    Sprite::ThornCaltrops, {1, 1, 0, 6, 40, PatternEffect::Damage, false, 1},
    ItemAction::Material, 10, 3, true, 0, 0, 0, 0, 0, SoundId::ThornScatter};

} // namespace

const RegionalItem* forest_ground_tool(ItemKind kind) {
    switch (kind) {
    case ItemKind::BirdSeed: return &seed;
    case ItemKind::ThornCaltrops: return &thorns;
    default: return nullptr;
    }
}

bool use_ground_tool(Game& game, int slot, Cell direction) {
    const Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const Item& item = *user.inventory.held();
    if (item.kind == ItemKind::BirdSeed)
        return plant_prop(game, user.cell + direction, PropKind::BirdSeed);
    const ItemPattern pattern = item_pattern(item);
    const Cell side{-direction.y, direction.x};
    bool used = false;
    // PLACEMENT: Only clear dry cells; a partial scatter still spends one handful.
    for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane)
        for (int reach = pattern.minimum; reach <= pattern.maximum; ++reach) {
            const Cell cell = user.cell + Cell{direction.x * reach + side.x * lane,
                                               direction.y * reach + side.y * lane};
            const Tile* tile = game.stage.at(cell);
            if (tile == nullptr || !walkable(*tile)) break;
            if (!plant_prop(game, cell, PropKind::Thorns)) continue;
            // VARIANT: Thorn props store their per-contact damage, including item modifiers.
            game.stage.at(cell)->prop.variant = static_cast<std::uint8_t>(std::clamp(pattern.damage, 1, 255));
            used = true;
        }
    return used;
}
