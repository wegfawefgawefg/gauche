#pragma once
#include "catalog.hpp"
#include <array>
const RegionalItem* barricade_item(ItemKind kind);
std::array<Cell,3> barricade_cells(Cell origin,Cell direction);
bool place_barricade(Game& game,int slot,Cell direction);
