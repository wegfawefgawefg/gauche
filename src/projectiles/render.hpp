#pragma once
#include "projectile.hpp"
#include "../lighting/field.hpp"
#include "../view.hpp"

void draw_projectile(SDL_Renderer* renderer, const GameGraphics& graphics,
                     const Entity& shot, const Game& game, ViewCamera camera,
                     float zoom, const LightingCache& lighting);
