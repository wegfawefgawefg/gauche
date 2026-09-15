#pragma once
#include "../game.hpp"

// Dispatch one-shot Industry tools. Caller owns cooldown installation, sound and
// resource completion; continuous tools and ordinary melee/guns have other owners.
bool use_industrial_tool(Game& game, int user_slot, Cell direction, int range, int& cooldown);
