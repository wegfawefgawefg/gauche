#pragma once

#include "../lighting/render.hpp"

void draw_plant_lash(SDL_Renderer* renderer, const Game& game, ViewCamera camera,
                     float zoom, const LightingCache& lighting);

void draw_root_head(SDL_Renderer* renderer, const Entity& root, SDL_FRect rect, LightColor light);
