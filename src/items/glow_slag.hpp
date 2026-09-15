#pragma once
#include "catalog.hpp"
constexpr int glow_slag_ticks=1200;
const RegionalItem* glow_slag_item(ItemKind kind);
bool glowing_slag(const Item& item);
void step_glow_slag(Game& game,Item& item,Cell cell,bool wet);
bool reheat_glow_slag(Game& game,int slot);
