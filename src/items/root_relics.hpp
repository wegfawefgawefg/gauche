#pragma once
#include "catalog.hpp"

const RegionalItem* forest_root_relic(ItemKind kind);
bool launch_root_relic(Game& game, int owner_slot, const Item& item, Cell direction);
