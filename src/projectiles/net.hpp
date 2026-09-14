#pragma once
#include "projectile.hpp"

bool launch_net(Game& game, int owner_slot, const Item& item, Cell direction);
void step_net(Game& game, int slot);
bool net_target(const Entity& actor);
int net_open_lanes(const Game& game, Cell center, Cell side, int width, int mask);
