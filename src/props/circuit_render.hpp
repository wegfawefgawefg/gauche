#pragma once
#include "render.hpp"

void draw_wire_connections(SDL_Renderer* renderer, const Stage& stage, Cell cell,
                           SDL_FRect rect, LightColor light);
