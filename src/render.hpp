#pragma once

#include "game.hpp"

struct Cosmetics;
struct PointerState;

void render_game(SDL_Renderer* renderer, const GameGraphics& graphics,
                 const Game& game, int local_owner, bool can_restart, float zoom,
                 const Cosmetics* cosmetics, const PointerState& pointer);
void render_title_backdrop(SDL_Renderer* renderer, const GameGraphics& graphics,
                           const Game& scene);
