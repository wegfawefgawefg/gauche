#pragma once
#include "../game.hpp"
#include "../lighting/field.hpp"
void draw_crane_parts(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                      ViewCamera camera,float zoom,const LightingCache& lighting,bool shadows);
