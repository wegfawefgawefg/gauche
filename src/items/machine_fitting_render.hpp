#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../lighting/field.hpp"
void draw_machine_fitting(tr::Renderer* renderer,const GameGraphics& graphics,const Entity& machine,SDL_FRect rect,LightColor light);
