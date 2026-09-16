#pragma once
#include "../lighting/render.hpp"

void draw_gunner_aim(SDL_Renderer* renderer,const Game& game,ViewCamera camera,
                     float zoom,const LightingCache& lighting);
