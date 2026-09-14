#pragma once

#include <SDL3/SDL.h>

inline constexpr float ui_scale = .72F;
inline constexpr float modal_left = 640.0F * (1 - ui_scale) * .5F;
inline constexpr float modal_top = 360.0F * (1 - ui_scale) * .5F;

struct HudScale {
    SDL_Renderer* renderer;
    float x = 1, y = 1;
    explicit HudScale(SDL_Renderer* target) : renderer(target) {
        SDL_GetRenderScale(renderer, &x, &y);
        SDL_SetRenderScale(renderer, x * ui_scale, y * ui_scale);
    }
    ~HudScale() { SDL_SetRenderScale(renderer, x, y); }
};
