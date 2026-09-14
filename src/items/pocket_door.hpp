#pragma once

#include "catalog.hpp"
#include <optional>

const RegionalItem* forest_pocket_door(ItemKind kind);
std::optional<Cell> pocket_door_landing(const Game& game, Cell from, Cell direction, int reach);
bool place_pocket_door(Game& game, int slot, Cell direction);
void fold_unused_door(Item& item);
