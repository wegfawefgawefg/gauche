#pragma once
#include "../view.hpp"
#include "../lighting/field.hpp"
#include "../game.hpp"

void draw_warden_charge(SDL_Renderer* renderer, const GameGraphics& graphics,
    const Game& game, const Entity& warden, ViewCamera camera, float zoom, const LightingCache& lighting);
