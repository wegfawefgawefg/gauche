#pragma once
#include "projectile.hpp"

bool launch_ice_brick(Game& game, int owner_slot, const Item& item, Cell direction);
void step_ice_brick_flight(Game& game, int slot);
