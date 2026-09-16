#pragma once
#include "../render.hpp"
#include "../lighting/field.hpp"
void draw_ice_pillar(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                     Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting);
void draw_pillar_shadow(SDL_Renderer* renderer,const Stage& stage,Cell cell,ViewCamera camera,float zoom);
