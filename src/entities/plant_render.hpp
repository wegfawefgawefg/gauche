#pragma once
#include "renderer/device.hpp"

#include "../lighting/render.hpp"

void draw_plant_lash(tr::Renderer* renderer, const Game& game, ViewCamera camera,
                     float zoom, const LightingCache& lighting);

void draw_root_head(tr::Renderer* renderer, const Entity& root, SDL_FRect rect, LightColor light);
