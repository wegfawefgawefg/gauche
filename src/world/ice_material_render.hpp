#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../lighting/render.hpp"

void draw_ice_material_details(tr::Renderer* renderer,const GameGraphics& graphics,
    const Stage& stage,Cell cell,SDL_FRect rect,const LightingCache& lighting);
