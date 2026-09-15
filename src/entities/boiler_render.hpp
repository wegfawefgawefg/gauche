#pragma once
#include "../game.hpp"
#include "../lighting/field.hpp"

void draw_boiler_details(SDL_Renderer* renderer, const GameGraphics& graphics,
                        const Entity& tank, SDL_FRect rect, LightColor light);
