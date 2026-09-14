#pragma once
#include "../lighting/render.hpp"

void draw_item_float(SDL_Renderer* renderer, const GameGraphics& graphics,
    const Entity& cargo, ViewCamera camera, float zoom, const LightingCache& lighting);
