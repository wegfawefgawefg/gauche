#pragma once
#include "catalog.hpp"
#include <vector>

const RegionalItem* brine_item(ItemKind kind);
std::vector<Cell> brine_cells(const Game& game, const Item& item, Cell center);
void brine_impact(Game& game, const Item& item, Cell center);
