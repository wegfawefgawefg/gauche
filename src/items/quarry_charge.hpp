#pragma once
#include "catalog.hpp"
const RegionalItem* quarry_charge_item(ItemKind kind);
bool place_quarry_charge(Game& game,int slot);
void step_quarry_charge(Game& game,int slot);
bool quarry_wall(const Stage& stage,Cell cell);
bool valid_quarry_charge(const Entity& actor);
Handle arm_quarry_charge(Game& game,int slot,Item item,Cell direction,int fuse_ticks);
