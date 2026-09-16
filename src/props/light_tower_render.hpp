#pragma once
#include "../render.hpp"
#include "../lighting/field.hpp"
void draw_light_tower(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting);
void draw_tower_ground(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting);
