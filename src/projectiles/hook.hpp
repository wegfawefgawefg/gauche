#pragma once
#include "projectile.hpp"

bool launch_hook(Game& game, int owner_slot, const Item& item, Cell direction);
void step_hook(Game& game, int slot);
