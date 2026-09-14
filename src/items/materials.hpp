#pragma once
#include "catalog.hpp"

const RegionalItem* forest_material_item(ItemKind kind);
bool use_material_item(Game& game, int slot, Cell direction);
void material_impact(Game& game, const Item& item, Cell cell);
