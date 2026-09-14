#include "eel_render.hpp"
#include "glass_eel.hpp"

#include <cmath>

void apply_eel_pose(const Entity& eel, std::uint64_t tick, SDL_FRect& rect, double& angle) {
    if (eel.kind != EntityKind::GlassEel) return;
    angle = std::atan2(static_cast<double>(eel.facing.y), static_cast<double>(eel.facing.x)) * 180.0 / 3.141592653589793;
    if (eel.health <= 0 || eel.sleep_ticks > 0 || eel.stun_ticks > 0) return;
    const double phase = static_cast<double>(tick % 600) * .3;
    if (eel.sprite == Sprite::EelStranded) angle += std::sin(phase) * 14;
    else if (eel.label_a == EelCharge) {
        const float pulse = .94F + .06F * static_cast<float>(std::sin(phase * 2));
        rect.x += rect.w * (1 - pulse) * .5F;
        rect.y += rect.h * (1 - pulse) * .5F;
        rect.w *= pulse; rect.h *= pulse;
    }
}
