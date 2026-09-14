#pragma once
#include "../game.hpp"
#include "../lighting/field.hpp"

void draw_knight_shield(SDL_Renderer* renderer, const GameGraphics& graphics,
    const Entity& knight, SDL_FRect rect, LightColor light);
