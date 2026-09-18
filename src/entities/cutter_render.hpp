#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../lighting/field.hpp"
void draw_cutter_details(tr::Renderer* renderer,const GameGraphics& graphics,const Entity& cutter,
    SDL_FRect rect,LightColor light);
