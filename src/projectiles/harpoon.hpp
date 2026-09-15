#pragma once
#include "projectile.hpp"

bool launch_harpoon(Game& game, int owner, Item& item, Cell direction);
void release_harpoon(Game& game, Handle handle);
void step_harpoon(Game& game, int slot);
bool valid_harpoon_state(const Entity& entity);
