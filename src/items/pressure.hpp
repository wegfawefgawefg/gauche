#pragma once
#include "catalog.hpp"

const RegionalItem* pressure_item(ItemKind kind);
bool use_pressure_item(Game& game, int slot, Cell direction);
Item removable_valve(const Game& game, Cell cell);
int release_valve(Game& game, Cell source, Cell destination);
