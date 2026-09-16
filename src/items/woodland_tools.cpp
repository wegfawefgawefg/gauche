#include "woodland_tools.hpp"
#include "../props/growth.hpp"

namespace {

// TOOLS: Distinct ways to change the room or preserve carried equipment.
constexpr RegionalItem claws{"Digging Claws", "200 quick scratches for digging and fighting. Dig power 1; 8 damage. Pierces a snail's shell.",
    Sprite::DiggingClaws, {1, 1, 0, 8, 18, PatternEffect::Damage},
    ItemAction::Melee, 17, 1, false, 200, 0, 0, 0, 1, SoundId::ClawScratch};
constexpr RegionalItem resin{"Resin Glue", "Fully repair the carried item with the most condition lost. Cannot restore uses or ammo.",
    Sprite::ResinGlue, {0, 0, 0, 0, 60, PatternEffect::Utility},
    ItemAction::Material, 14, 3, true, 0, 0, 0, 0, 0, SoundId::ResinRepair};
constexpr RegionalItem seeds{"Seed Bag", "Dry ground grows 40-HP cover in 3s. Cut or burn it. Won't grow under actors.",
    Sprite::SeedBag, {1, 1, 0, 0, 35, PatternEffect::Utility},
    ItemAction::Material, 8, 1, false, 6, 0, 0, 0, 0, SoundId::SeedPlant};
constexpr RegionalItem lantern{"Lantern Seed", "Plant a 12-HP glowing flower. A small steady light that attracts moths. Burnable.",
    Sprite::LanternSeed, {1, 1, 0, 0, 35, PatternEffect::Utility},
    ItemAction::Material, 8, 4, true, 0, 0, 0, 0, 0, SoundId::LanternPlant};

} // namespace

const RegionalItem* woodland_tool(ItemKind kind) {
    switch (kind) {
    case ItemKind::DiggingClaws: return &claws;
    case ItemKind::ResinGlue: return &resin;
    case ItemKind::SeedBag: return &seeds;
    case ItemKind::LanternSeed: return &lantern;
    default: return nullptr;
    }
}

int resin_repair_slot(const Inventory& inventory) {
    int best = -1, lost = 0;
    for (int slot = 0; slot < quick_slots; ++slot) {
        const Item& item = inventory.slots[static_cast<std::size_t>(slot)];
        if (item.kind == ItemKind::None || item.count <= 0 || item.max_durability <= 0) continue;
        const int missing = item.max_durability - item.durability;
        if (missing > lost) { best = slot; lost = missing; }
    }
    return best;
}

bool use_woodland_tool(Game& game, int slot, Cell direction) {
    Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const ItemKind kind = user.inventory.held()->kind;
    if (kind == ItemKind::ResinGlue) {
        const int repair = resin_repair_slot(user.inventory);
        if (repair < 0) return false;
        Item& target = user.inventory.slots[static_cast<std::size_t>(repair)];
        target.durability = target.max_durability;
        return true;
    }
    return plant_prop(game, user.cell + direction,
        kind == ItemKind::SeedBag ? PropKind::Shoot : PropKind::LanternPlant);
}
