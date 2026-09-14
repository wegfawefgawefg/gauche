#pragma once
#include "catalog.hpp"

constexpr int scoop_snow_capacity = 12;
const RegionalItem* snow_tool_item(ItemKind kind);
bool use_snow_scoop(Game& game, int slot, Cell direction);
bool pack_snowball(Game& game, int slot);
