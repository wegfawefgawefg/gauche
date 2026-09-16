#pragma once
#include "catalog.hpp"
const RegionalItem* coal_item(ItemKind kind);
bool use_coal(Game& game, int slot, Cell direction);
bool coal_lava_contact(Game& game,const Item& item,Cell cell);
void step_ground_coal(Game& game,int slot);
