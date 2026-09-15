#pragma once
#include "../game.hpp"

// Tile current: 0 none, 1 east, 2 south, 3 west, 4 north. Frozen water retains it.
Cell water_current(const Tile& tile);
void step_water_currents(Game& game);
struct FloorPlan;
void place_water_currents(Game& game, const FloorPlan& plan);
