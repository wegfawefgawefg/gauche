#include "fish.hpp"

namespace {

constexpr RegionalItem smoked_fish{"Smoked Fish", "Eat for 14 HP, or drop as bait. Eels follow fish through water. Feeding buys a pause, not a canceled shock.",
    Sprite::SmokedFish, {0,0,0,0,90,PatternEffect::Heal,false,0,14},
    ItemAction::Food,6,6,true,0,0,0,0,0,SoundId::FishEat};

} // namespace

const RegionalItem* fish_item(ItemKind kind) {
    return kind == ItemKind::SmokedFish ? &smoked_fish : nullptr;
}
