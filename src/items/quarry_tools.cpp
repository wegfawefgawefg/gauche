#include "quarry_tools.hpp"

namespace {
constexpr RegionalItem chisel{"Chisel", "Quick jab. Double damage to ice walls and blocks. Dig power 1. Wears per swing; glue repairs it.",
    Sprite::Chisel, {1, 1, 0, 9, 18, PatternEffect::Damage},
    ItemAction::Melee, 12, 1, false, 0, 0, 0, 0, 1, SoundId::ChiselJab, 40};
constexpr RegionalItem brick{"Ice Brick", "Tap-release: place cover. Hold 0.3s-release: throw. 35 HP cover lasts 10s. Heat melts it; throws shatter.",
    Sprite::IceBlock, {1, 3, 0, 14, 30, PatternEffect::Damage, true},
    ItemAction::Throw, 8, 3, true, 0, 0, 0, 0, 0, SoundId::IceBrickThrow};
}

const RegionalItem* quarry_item(ItemKind kind) {
    if (kind == ItemKind::Chisel) return &chisel;
    if (kind == ItemKind::IceBrick) return &brick;
    return nullptr;
}
