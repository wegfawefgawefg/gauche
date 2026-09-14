#pragma once
#include "catalog.hpp"

const RegionalItem* cold_remedy_item(ItemKind kind);
bool use_cold_remedy(Game& game, int slot);
