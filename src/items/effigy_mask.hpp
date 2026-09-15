#pragma once
#include "catalog.hpp"

constexpr int mask_action_label=quick_slots+1;
const RegionalItem* effigy_mask_item(ItemKind kind);
bool effigy_mask_active(const Entity& user);
int effigy_mask_ticks(const Item& item);
bool step_effigy_mask(Game& game,int slot,const Input& input);
void stow_effigy_mask(Item& item,bool held);
