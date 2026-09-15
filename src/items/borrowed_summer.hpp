#pragma once
#include "catalog.hpp"

const RegionalItem* borrowed_summer_item(ItemKind kind);
bool use_borrowed_summer(Game& game, int slot);
void step_summer_auras(Game& game);
