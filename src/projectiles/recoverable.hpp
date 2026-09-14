#pragma once
#include "projectile.hpp"

bool launch_recoverable(Game& game, int owner_slot, Item& item, Cell direction);
void step_recoverable(Game& game, int slot);
void finish_recoverables(Game& game);
