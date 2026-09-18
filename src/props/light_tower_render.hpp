#pragma once
#include "renderer/device.hpp"
#include "../render.hpp"
#include "../lighting/field.hpp"
void draw_light_tower(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting);
void draw_tower_ground(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting);
