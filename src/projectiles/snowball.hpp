#pragma once
#include "../game.hpp"

bool launch_snowball(Game& game, int owner, const Item& item, Cell direction);
void step_snowball(Game& game, int slot);
