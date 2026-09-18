#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../lighting/field.hpp"

void draw_brick_prepare(tr::Renderer* renderer, const Entity& user,
    SDL_FRect rect, LightColor light);
