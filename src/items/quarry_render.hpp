#pragma once
#include "../game.hpp"
#include "../lighting/field.hpp"

void draw_brick_prepare(SDL_Renderer* renderer, const Entity& user,
    SDL_FRect rect, LightColor light);
