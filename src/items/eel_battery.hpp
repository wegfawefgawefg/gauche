#pragma once
#include "catalog.hpp"

const RegionalItem* eel_battery_item(ItemKind kind);
bool use_eel_battery(Game& game, int slot, Cell direction);
