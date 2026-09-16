#pragma once
#include "catalog.hpp"
const RegionalItem* lunch_tin_item(ItemKind kind);
bool use_lunch_tin(Game& game,int slot,Cell direction);
void land_lunch_tin(Game& game,Cell cell);
