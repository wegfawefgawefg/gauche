#pragma once

#include "../game.hpp"
#include "../lighting/render.hpp"

void apply_flight_pose(const Entity& bird, std::uint64_t tick, SDL_FRect& rect, double& angle);
void draw_owl_landing(SDL_Renderer* renderer, const GameGraphics& graphics, const Game& game,
    ViewCamera camera, float zoom, const LightingCache& lighting);
