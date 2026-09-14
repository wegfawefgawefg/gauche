#pragma once
#include "catalog.hpp"

const RegionalItem* forest_ground_tool(ItemKind kind);
bool use_ground_tool(Game& game, int slot, Cell direction);
