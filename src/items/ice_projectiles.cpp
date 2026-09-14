#include "ice_projectiles.hpp"

namespace {

constexpr RegionalItem needle{"Ice Needle", "Throw, chill, recover. Shields block chill. Melts on a hot landing; flames resist cold.",
    Sprite::IceNeedle, {1, 7, 0, 8, 30, PatternEffect::Damage, true},
    ItemAction::Throw, 4, 8, true, 0, 0, 0, 0, 0, SoundId::IceNeedleThrow};

} // namespace

const RegionalItem* ice_projectile_item(ItemKind kind) {
    return kind == ItemKind::IceNeedle ? &needle : nullptr;
}
