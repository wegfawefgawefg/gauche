#pragma once
#include "catalog.hpp"
#include "heated_water.hpp"

constexpr int kettle_heat_ticks = water_heat_ticks;
constexpr int kettle_cool_ticks = water_cool_ticks;
const RegionalItem* kettle_item(ItemKind kind);
bool use_kettle(Game& game, int slot, Cell direction);
std::vector<Cell> kettle_cells(const Game& game, const Item& item, Cell source, Cell direction);
