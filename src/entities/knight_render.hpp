#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../lighting/field.hpp"

void draw_knight_shield(tr::Renderer* renderer, const GameGraphics& graphics,
    const Entity& knight, SDL_FRect rect, LightColor light);
