#pragma once

#include "../lighting/render.hpp"
#include "../view.hpp"

void draw_enemy_intents(SDL_Renderer* renderer, const Game& game,
                        ViewCamera camera, float zoom, const LightingCache& lighting);
