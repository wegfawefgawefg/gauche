#include "fishing_line.hpp"

namespace {

constexpr RegionalItem line{"Fishing Line", "Stay still to reel loose loot to your feet. Actors and cover stop the hook. Moving cuts the line.",
    Sprite::FishingLine, {1, 6, 0, 0, 45, PatternEffect::Utility, true},
    ItemAction::Material, 8, 1, false, 20, 0, 0, 0, 0, SoundId::FishingCast};

} // namespace

const RegionalItem* fishing_line_item(ItemKind kind) {
    return kind == ItemKind::FishingLine ? &line : nullptr;
}
