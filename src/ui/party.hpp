#pragma once
#include "renderer/device.hpp"

#include "../game.hpp"
#include <SDL3/SDL.h>

const char* party_player_status(const Game& game, int owner);
void draw_party_status(tr::Renderer* renderer, const Game& game, int local_owner,
                       float x, float y, bool horizontal);
