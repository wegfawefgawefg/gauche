#pragma once
#include "projectile.hpp"

constexpr int prism_fuse_ticks = 90;
bool launch_prism_bomb(Game& game, int owner, const Item& item, Cell direction);
void step_prism_bomb(Game& game, int slot);
