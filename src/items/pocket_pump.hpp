#pragma once
#include "catalog.hpp"
constexpr int pump_capacity=1800;
constexpr int pump_portion=600;
const RegionalItem* pocket_pump_item(ItemKind kind);
const char* pump_contents(const Item& item);
bool valid_pocket_pump(const Item& item);
bool collect_pocket_pump(Game& game,int slot,Cell direction);
bool discharge_pocket_pump(Game& game,int slot);
bool step_pump_action(Game& game,int slot,const Input& input);
