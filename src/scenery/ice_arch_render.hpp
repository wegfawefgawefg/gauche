#pragma once
#include "renderer/device.hpp"
#include "roof_render.hpp"
void draw_ice_arch_row(tr::Renderer* renderer,const GameGraphics& graphics,const Stage& stage,
    const RoofSpan& roof,int row,const Entity* viewer,ViewCamera camera,float zoom,const LightingCache& lighting);
