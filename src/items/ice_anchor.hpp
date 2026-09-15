#pragma once
#include "catalog.hpp"

const RegionalItem* ice_anchor_item(ItemKind kind);
void init_ice_anchor(Entity& anchor);
void step_ice_anchor(Game& game,int slot);
bool step_anchor_action(Game& game,int slot,const Input& input);
void sync_ice_anchor(Game& game,Item& item);
void fold_ice_anchor(Item& item);
bool anchor_path_clear(const Game& game,Cell from,Cell to);
Cell anchor_next_cell(Cell from,Cell to);
bool valid_ice_anchor(const Entity& anchor);
bool hit_ice_anchor(Game& game,Cell cell,int damage,Cell source);
