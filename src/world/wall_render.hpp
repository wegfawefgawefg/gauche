#pragma once
#include "renderer/device.hpp"

#include "../lighting/render.hpp"

void draw_wall_contour(tr::Renderer* renderer, const Stage& stage, Cell cell,
                       SDL_FRect rect, const LightingCache& lighting, LightColor tint);
