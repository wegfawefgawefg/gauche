#pragma once

#include "../lighting/render.hpp"

void draw_wolf_call(SDL_Renderer* renderer, const GameGraphics& graphics,
                    const Entity& wolf, SDL_FRect rect, LightColor light);
