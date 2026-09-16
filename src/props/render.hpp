#pragma once

#include "../game.hpp"
#include "../view.hpp"
#include "../lighting/field.hpp"

void draw_props(SDL_Renderer* renderer, const GameGraphics& graphics, const Game& game,
                 ViewCamera camera, float zoom, const LightingCache& lighting, std::uint64_t tick=0);
