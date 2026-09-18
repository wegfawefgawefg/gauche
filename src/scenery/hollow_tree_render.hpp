#pragma once
#include "renderer/device.hpp"
#include "roof_render.hpp"
// Visual relief covers the raised crown and trunk, plus a soft approach band.
float hollow_tree_opacity(const RoofSpan& roof,Cell viewer);
void draw_hollow_tree_row(tr::Renderer* renderer,const GameGraphics& graphics,const Stage& stage,
    const RoofSpan& roof,int row,const Entity* viewer,ViewCamera camera,float zoom,const LightingCache& lighting);
