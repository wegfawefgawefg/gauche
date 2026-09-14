#pragma once
#include "../game.hpp"
#include "../view.hpp"
#include "../lighting/field.hpp"

void draw_drummer_warning(SDL_Renderer* renderer, const Game& game, const Entity& drummer,
    ViewCamera camera, float zoom, const LightingCache& lighting);
