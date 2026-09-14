#pragma once
#include "../lighting/render.hpp"

void apply_leech_pose(const Entity& leech, std::uint64_t tick, SDL_FRect& rect, double& angle);
void draw_leech_tether(SDL_Renderer* renderer, const Game& game, const Entity& leech,
    ViewCamera camera, float zoom, const LightingCache& lighting);
