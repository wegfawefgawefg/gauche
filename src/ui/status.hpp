#pragma once

#include "../game.hpp"

void draw_player_status(SDL_Renderer* renderer, const GameGraphics& graphics,
                         const Game& game, const Entity& player, float x, float bottom);
