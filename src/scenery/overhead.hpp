#pragma once
#include "../particles/system.hpp"

void draw_overhead(SDL_Renderer* renderer, const GameGraphics& graphics,
                    const Game& game, const Cosmetics* cosmetics, ViewCamera camera,
                    float zoom, const LightingCache& lighting);
