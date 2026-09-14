#pragma once
#include "catalog.hpp"

const RegionalItem* snow_globe_item(ItemKind kind);
bool use_snow_globe(Game& game, int slot, Cell direction);
