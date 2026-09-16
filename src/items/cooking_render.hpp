#pragma once
#include "../lighting/render.hpp"
void draw_cooking(SDL_Renderer* renderer,const GameGraphics& graphics,
                  const Entity& player,SDL_FRect rect,LightColor light);
