#pragma once

#include "catalog.hpp"

const RegionalItem* woodland_tool(ItemKind kind);
int resin_repair_slot(const Inventory& inventory);
bool use_woodland_tool(Game& game, int slot, Cell direction);
