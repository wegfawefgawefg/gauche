#pragma once
#include "renderer/device.hpp"
#include "../render.hpp"
#include "../lighting/field.hpp"
void draw_ant_ropes(tr::Renderer* renderer,const Game& game,const Entity& load,ViewCamera camera,float zoom,const LightingCache& lighting);
