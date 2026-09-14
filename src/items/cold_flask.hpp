#pragma once
#include "catalog.hpp"

const RegionalItem* cold_flask_item(ItemKind kind);
std::vector<Cell> cold_flask_cells(const Game& game, const Item& item, Cell center);
void cold_flask_impact(Game& game, const Item& item, Cell center);
