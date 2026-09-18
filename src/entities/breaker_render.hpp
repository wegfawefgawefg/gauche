#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../lighting/field.hpp"
void draw_breaker_shield(tr::Renderer* renderer,const GameGraphics& graphics,
    const Entity& guard,SDL_FRect rect,LightColor light);
