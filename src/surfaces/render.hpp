#pragma once
#include "../particles/system.hpp"

void draw_surfaces(SDL_Renderer* renderer, const Game& game, ViewCamera camera,
                    float zoom, const LightingCache& lighting, bool clouds);
void observe_surfaces(Cosmetics& cosmetics, const Game& game, Cell focus);
