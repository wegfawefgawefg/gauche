#pragma once
#include "roof_render.hpp"
void draw_hollow_tree_row(SDL_Renderer* renderer,const GameGraphics& graphics,const Stage& stage,
    const RoofSpan& roof,int row,const Entity* viewer,ViewCamera camera,float zoom,const LightingCache& lighting);
