#pragma once
#include "../game.hpp"
#include "../lighting/field.hpp"
void draw_cutter_details(SDL_Renderer* renderer,const GameGraphics& graphics,const Entity& cutter,
    SDL_FRect rect,LightColor light);
