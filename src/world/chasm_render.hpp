#pragma once
#include "../game.hpp"
#include "../lighting/render.hpp"

bool draw_chasm(SDL_Renderer* renderer,const Game& game,Cell cell,SDL_FRect rect,const LightingCache& lighting);
