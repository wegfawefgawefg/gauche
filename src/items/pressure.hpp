#pragma once
#include "catalog.hpp"

const RegionalItem* pressure_item(ItemKind kind);
bool use_pressure_item(Game& game, int slot, Cell direction);
