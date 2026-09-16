#pragma once
#include "../render.hpp"
#include "../lighting/field.hpp"
void draw_boiler_feeds(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
    ViewCamera camera,float zoom,const LightingCache& lighting);
void draw_boiler_water(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,const Entity& tank,SDL_FRect rect,LightColor light);
