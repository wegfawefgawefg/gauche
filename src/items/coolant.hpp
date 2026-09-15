#pragma once
#include "catalog.hpp"
const RegionalItem* coolant_item(ItemKind kind);
bool pour_coolant(Game& game,int slot,Cell direction);
