#pragma once
#include "catalog.hpp"
const RegionalItem* chain_hook_item(ItemKind kind);
void release_held_chain(Game& game,Entity& owner);
bool step_chain_action(Game& game,int slot,const Input& input);
