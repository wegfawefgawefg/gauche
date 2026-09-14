#include "shields.hpp"

namespace {

// LANTERN: A lighter shield; its emitter travels with the actual item instance.
constexpr RegionalItem lantern{"Shield Lantern",
    "Shove ahead and block facing hits for 0.25s. Damage wears the shield. Lights the way while held or dropped.",
    Sprite::ShieldLantern, {1, 1, 0, 0, 30, PatternEffect::Utility},
    ItemAction::Material, 24, 1, false, 0, 0, 0, 0, 0,
    SoundId::LanternRaise, 24, {5, 1200, {255, 191, 99}}};

} // namespace

const RegionalItem* forest_shield(ItemKind kind) {
    return kind == ItemKind::ShieldLantern ? &lantern : nullptr;
}
