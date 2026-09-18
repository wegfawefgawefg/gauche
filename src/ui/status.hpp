#pragma once
#include "renderer/device.hpp"

#include "../game.hpp"

void draw_player_status(tr::Renderer* renderer, const GameGraphics& graphics,
                         const Game& game, const Entity& player, float x, float bottom);
