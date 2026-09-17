#pragma once

#include "../game.hpp"
#include <SDL3/SDL.h>

const char* party_player_status(const Game& game, int owner);
void draw_party_status(SDL_Renderer* renderer, const Game& game, int local_owner,
                       float x, float y, bool horizontal);
