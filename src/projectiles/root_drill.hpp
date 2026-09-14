#pragma once
#include "projectile.hpp"

bool root_drill_blocked(const Game& game, Cell cell, const Item& item);
void step_root_drill(Game& game, int slot);
