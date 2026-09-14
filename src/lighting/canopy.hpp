#pragma once

#include "field.hpp"

float canopy_strength(const StageLight& light, float x, float y, std::uint64_t tick);
void project_canopy(LightingCache& cache, const Game& game);
void draw_canopy_shafts(SDL_Renderer* renderer, const Game& game,
                        ViewCamera camera, float zoom);
