#pragma once
#include "renderer/device.hpp"
#include "../lighting/render.hpp"
void draw_cooking(tr::Renderer* renderer,const GameGraphics& graphics,
                  const Entity& player,SDL_FRect rect,LightColor light);
