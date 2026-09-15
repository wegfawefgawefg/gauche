#pragma once
#include "catalog.hpp"
constexpr int tension_arm_ticks=18;
const RegionalItem* tension_spring_item(ItemKind kind);
bool place_tension_spring(Game& game,int slot,Cell direction);
