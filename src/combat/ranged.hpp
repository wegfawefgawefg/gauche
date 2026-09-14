#pragma once

#include "../game.hpp"

void fire_bullet(Game& game, int owner_slot, Cell source, Cell direction,
                 const Item& weapon, bool muzzle);
