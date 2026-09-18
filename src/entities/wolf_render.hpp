#pragma once
#include "renderer/device.hpp"

#include "../lighting/render.hpp"

void pose_canine(const Entity& wolf,SDL_FRect& rect,double& angle);
void draw_wolf_call(tr::Renderer* renderer, const GameGraphics& graphics,
                    const Entity& wolf, SDL_FRect rect, LightColor light);
