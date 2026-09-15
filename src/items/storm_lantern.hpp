#pragma once
#include "catalog.hpp"

constexpr int lantern_fuel_ticks=7200;
const RegionalItem* storm_lantern_item(ItemKind kind);
LightEmitter lantern_light(const Item& item);
Sprite lantern_sprite(const Item& item);
bool step_lantern_action(Game& game,int slot,const Input& input);
void step_lantern_fuel(Game& game,Item& item,Cell cell,bool exposed,bool held);
