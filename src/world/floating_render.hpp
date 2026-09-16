#pragma once
#include "../lighting/render.hpp"

void draw_item_float(SDL_Renderer* renderer, const GameGraphics& graphics,
    const Entity& cargo, ViewCamera camera, float zoom, const LightingCache& lighting);
void draw_personal_float(SDL_Renderer* renderer, const GameGraphics& graphics,
    const Game& game, const Entity& actor, ViewCamera camera, float zoom, const LightingCache& lighting);
