#pragma once
#include "renderer/device.hpp"

#include <SDL3/SDL.h>

inline constexpr float ui_scale = .72F;
inline constexpr float modal_left = 640.0F * (1 - ui_scale) * .5F;
inline constexpr float modal_top = 360.0F * (1 - ui_scale) * .5F;

struct HudScale {
    tr::Renderer* renderer;
    float x = 1, y = 1;
    explicit HudScale(tr::Renderer* target) : renderer(target) {
        tr::get_scale(renderer, &x, &y);
        tr::set_scale(renderer, x * ui_scale, y * ui_scale);
    }
    ~HudScale() { tr::set_scale(renderer, x, y); }
};
