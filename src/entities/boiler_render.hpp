#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../lighting/field.hpp"

void draw_boiler_details(tr::Renderer* renderer, const GameGraphics& graphics,
                        const Entity& tank, SDL_FRect rect, LightColor light);
