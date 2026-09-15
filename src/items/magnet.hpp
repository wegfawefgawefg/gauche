#pragma once
#include "catalog.hpp"

const RegionalItem* magnet_item(ItemKind kind);
bool magnetic_item(ItemKind kind);
bool pull_magnetic_item(Game& game,int slot,Cell direction);
bool step_magnet(Game& game,int slot,const Input& input);
