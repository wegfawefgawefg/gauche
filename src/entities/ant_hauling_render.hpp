#pragma once
#include "../render.hpp"
#include "../lighting/field.hpp"
void draw_ant_ropes(SDL_Renderer* renderer,const Game& game,const Entity& load,ViewCamera camera,float zoom,const LightingCache& lighting);
