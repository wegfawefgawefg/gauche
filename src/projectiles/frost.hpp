#pragma once
#include "../game.hpp"

constexpr int frost_puff_step_ticks = 6;
bool launch_frost_puff(Game& game, int owner, Cell direction, int reach);
void step_frost_puff(Game& game, int slot);
