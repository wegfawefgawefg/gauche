#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../lighting/field.hpp"
void draw_crane_parts(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
                      ViewCamera camera,float zoom,const LightingCache& lighting,bool shadows,const Entity* only=nullptr);
