#pragma once
#include "catalog.hpp"

constexpr int kettle_heat_ticks = 90;
constexpr int kettle_cool_ticks = 1800;
const RegionalItem* kettle_item(ItemKind kind);
void step_kettle(Game& game, Item& item, Cell cell, bool wet);
bool use_kettle(Game& game, int slot, Cell direction);
std::vector<Cell> kettle_cells(const Game& game, const Item& item, Cell source, Cell direction);
