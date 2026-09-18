#pragma once
#include "renderer/device.hpp"
#include "../view.hpp"
#include "../lighting/field.hpp"
#include "../game.hpp"

void draw_mason_block(tr::Renderer* renderer, const GameGraphics& graphics,
    const Entity& mason, SDL_FRect rect, LightColor light);
