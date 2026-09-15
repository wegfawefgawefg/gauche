#pragma once
#include "catalog.hpp"
#include <array>

const RegionalItem* snow_shelter_item(ItemKind kind);
std::array<Cell,2> snow_shelter_cells(Cell origin,Cell direction);
bool place_snow_shelter(Game& game,int slot,Cell direction);
