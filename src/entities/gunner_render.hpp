#pragma once
#include "renderer/device.hpp"
#include "../lighting/render.hpp"

void draw_gunner_aim(tr::Renderer* renderer,const Game& game,ViewCamera camera,
                     float zoom,const LightingCache& lighting);
