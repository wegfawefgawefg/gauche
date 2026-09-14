#pragma once
#include "../view.hpp"
#include "../lighting/field.hpp"
#include "../game.hpp"

void draw_mason_block(SDL_Renderer* renderer, const GameGraphics& graphics,
    const Entity& mason, SDL_FRect rect, LightColor light);
