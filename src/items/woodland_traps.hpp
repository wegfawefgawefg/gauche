#pragma once
#include "catalog.hpp"

const RegionalItem* forest_trap_item(ItemKind kind);
bool place_woodland_trap(Game& game, int owner_slot, Cell direction);
