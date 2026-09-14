#include "catalog.hpp"

namespace {
constexpr RegionalItem egg{"Egg", "A fresh egg from a nest. Eat for 3 HP, or fry it at a campfire for a better meal.",
    Sprite::Egg, {0, 0, 0, 0, 35, PatternEffect::Heal, false, 0, 3},
    ItemAction::Food, 2, 12, true, 0, 0, 0, 0, 0, SoundId::EggEat};
constexpr RegionalItem fried{"Fried Egg", "A warm little meal. Restore 12 HP. Made by bringing an egg to a campfire.",
    Sprite::FriedEgg, {0, 0, 0, 0, 60, PatternEffect::Heal, false, 0, 12},
    ItemAction::Food, 5, 12, true, 0, 0, 0, 0, 0, SoundId::EggEat};
}

const RegionalItem* forest_food_item(ItemKind kind) {
    switch (kind) {
    case ItemKind::Egg: return &egg;
    case ItemKind::FriedEgg: return &fried;
    default: return nullptr;
    }
}
