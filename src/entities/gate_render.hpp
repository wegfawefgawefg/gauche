#pragma once
#include "../render.hpp"
#include "../lighting/render.hpp"

void draw_gate(SDL_Renderer* renderer,const GameGraphics& graphics,const Entity& gate,
    ViewCamera camera,float zoom,const LightingCache& lighting);
