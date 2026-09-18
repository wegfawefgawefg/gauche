#pragma once
#include "renderer/device.hpp"
#include "../render.hpp"
#include "../lighting/field.hpp"
void draw_boiler_feeds(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
    ViewCamera camera,float zoom,const LightingCache& lighting);
void draw_boiler_water(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,const Entity& tank,SDL_FRect rect,LightColor light);
