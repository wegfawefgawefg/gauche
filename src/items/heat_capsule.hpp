#pragma once
#include "catalog.hpp"

const RegionalItem* heat_capsule_item(ItemKind kind);
std::vector<Cell> heat_capsule_cells(const Game& game, const Item& item, Cell center);
bool use_heat_capsule(Game& game, int slot);
