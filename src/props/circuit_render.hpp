#pragma once
#include "renderer/device.hpp"
#include "render.hpp"

void draw_wire_connections(tr::Renderer* renderer, const Stage& stage, Cell cell,
                           SDL_FRect rect, LightColor light);
