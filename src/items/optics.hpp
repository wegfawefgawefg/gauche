#pragma once
#include "catalog.hpp"

const RegionalItem* optics_item(ItemKind kind);
bool place_optic(Game& game, int slot, Cell direction);
bool rotate_mirror(Game& game, int slot);
