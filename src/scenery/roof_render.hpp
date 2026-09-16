#pragma once
#include "roof.hpp"
#include "../lighting/render.hpp"

bool reveal_roof(const RoofSpan& roof,Cell viewer);
void draw_roofs(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                const Entity* viewer,ViewCamera camera,float zoom,const LightingCache& lighting);
