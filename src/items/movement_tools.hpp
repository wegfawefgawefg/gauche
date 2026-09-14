#pragma once
#include "catalog.hpp"

const RegionalItem* forest_movement_item(ItemKind kind);
bool use_movement_tool(Game& game, int slot, Cell direction);
