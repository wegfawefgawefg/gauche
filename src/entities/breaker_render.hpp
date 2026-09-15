#pragma once
#include "../game.hpp"
#include "../lighting/field.hpp"
void draw_breaker_shield(SDL_Renderer* renderer,const GameGraphics& graphics,
    const Entity& guard,SDL_FRect rect,LightColor light);
