#pragma once

#include "game.hpp"

void render_game(SDL_Renderer* renderer, const GameGraphics& graphics,
                 const Game& game, int local_owner, bool can_restart, float zoom);
void render_title_backdrop(SDL_Renderer* renderer, const GameGraphics& graphics,
                           const Game& scene);
