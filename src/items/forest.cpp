#include "catalog.hpp"
#include "materials.hpp"
#include "woodland_tools.hpp"

namespace {

// DEFINITIONS: Data shared by initialization, comparison cards and combat dispatch.
constexpr RegionalItem rock{"Throwing Rock", "Throw itself along a line. Recover it at the impact; 8 damage.",
    Sprite::ThrowingRock, {1, 6, 0, 8, 30, PatternEffect::Damage, true},
    ItemAction::Throw, 3, 6, true, 0, 0, 0, 60, 0, SoundId::RockThrow};
constexpr RegionalItem hatchet{"Hatchet", "Cleave three adjacent cells. Triple damage to timber, trees, crates and logs; cannot dig stone.",
    Sprite::Hatchet, {1, 1, 0, 16, 35, PatternEffect::Damage, false, 1},
    ItemAction::Melee, 14, 1, false, 35, 0, 0, 60, 0, SoundId::AxeSwing};
constexpr RegionalItem spear{"Hunting Spear", "Thrust through two cells, piercing actors along the shaft.",
    Sprite::HuntingSpear, {1, 2, 0, 12, 28, PatternEffect::Damage, false, 0, 0, true},
    ItemAction::Melee, 15, 1, false, 45, 0, 0, 60, 0, SoundId::SpearThrust};
constexpr RegionalItem crossbow{"Crossbow", "One traveling bolt, 32 damage on impact. Deliberate mechanical reload.",
    Sprite::Crossbow, {1, 14, 0, 32, 40, PatternEffect::Damage, true},
    ItemAction::Gun, 30, 1, false, 0, 1, 16, 75, 0, SoundId::CrossbowShot};
constexpr RegionalItem blunderbuss{"Blunderbuss", "Three short lanes, 10 damage each. Two shots; recoil steps you back.",
    Sprite::Blunderbuss, {1, 5, 0, 10, 40, PatternEffect::Damage, true, 1},
    ItemAction::Gun, 32, 1, false, 0, 2, 14, 85, 0, SoundId::BlunderShot};
constexpr RegionalItem maul{"Wooden Maul", "A slow 28-damage blow that shoves. An immovable surface crushes the victim.",
    Sprite::WoodenMaul, {1, 1, 0, 28, 60, PatternEffect::Damage},
    ItemAction::Melee, 23, 1, false, 24, 0, 0, 60, 0, SoundId::MaulSwing};
constexpr RegionalItem rake{"Rake", "Pull loose debris into a pile. Sweep three cells of brittle growth; 400 uses.",
    Sprite::Rake, {1, 1, 0, 7, 24, PatternEffect::Damage, false, 1},
    ItemAction::Melee, 9, 1, false, 400, 0, 0, 60, 0, SoundId::RakeSweep};
constexpr RegionalItem knife{"Flint Knife", "Fast 7-damage stab. Double damage against sleepers or from behind.",
    Sprite::FlintKnife, {1, 1, 0, 7, 12, PatternEffect::Damage},
    ItemAction::Melee, 12, 1, false, 60, 0, 0, 60, 0, SoundId::KnifeStab};

} // namespace

const RegionalItem* regional_item(ItemKind kind) {
    if (const RegionalItem* tool = woodland_tool(kind)) return tool;
    if (const RegionalItem* food = forest_food_item(kind)) return food;
    if (const RegionalItem* material = forest_material_item(kind)) return material;
    switch (kind) {
    case ItemKind::ThrowingRock: return &rock;
    case ItemKind::Hatchet: return &hatchet;
    case ItemKind::HuntingSpear: return &spear;
    case ItemKind::Crossbow: return &crossbow;
    case ItemKind::Blunderbuss: return &blunderbuss;
    case ItemKind::WoodenMaul: return &maul;
    case ItemKind::Rake: return &rake;
    case ItemKind::FlintKnife: return &knife;
    default: return nullptr;
    }
}
