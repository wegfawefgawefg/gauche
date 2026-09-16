#pragma once
#include "../render.hpp"
#include "../lighting/field.hpp"
void draw_tall_mushroom(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting,const Entity* viewer);
