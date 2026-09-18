#pragma once
#include "renderer/device.hpp"
#include "../render.hpp"
#include "../lighting/field.hpp"
void draw_ice_pillar(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
                     Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting);
void draw_pillar_shadow(tr::Renderer* renderer,const Stage& stage,Cell cell,ViewCamera camera,float zoom);
