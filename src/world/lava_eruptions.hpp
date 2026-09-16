#pragma once
#include "../game.hpp"

constexpr int max_lava_vents=12,lava_active_budget=3;
constexpr int lava_warning_ticks=72,lava_flight_ticks=36;
void place_lava_vents(Game& game);
void step_lava_eruptions(Game& game);
bool valid_lava_vents(const Game& game);
