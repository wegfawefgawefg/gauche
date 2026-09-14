#pragma once
#include "../game.hpp"

enum SnowPhase { SnowBurrow, SnowWarn, SnowExposed, SnowDive };
void init_snow_burrower(Entity& burrower);
void step_snow_burrower(Game& game, int slot);
void expose_snow_burrower(Entity& burrower);
