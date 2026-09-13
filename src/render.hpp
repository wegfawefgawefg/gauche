#pragma once

#include "game.hpp"

void render_game(SDL_Renderer* renderer, const GameGraphics& graphics,
                 const Game& game, int local_owner, bool can_restart);
