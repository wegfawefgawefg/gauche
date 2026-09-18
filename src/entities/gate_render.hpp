#pragma once
#include "renderer/device.hpp"
#include "../render.hpp"
#include "../lighting/render.hpp"

void draw_gate(tr::Renderer* renderer,const GameGraphics& graphics,const Entity& gate,
    ViewCamera camera,float zoom,const LightingCache& lighting);
