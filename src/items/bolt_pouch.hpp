#pragma once
#include "catalog.hpp"

const RegionalItem* bolt_pouch_item(ItemKind kind);
bool throw_bolts(Game& game,int owner,Cell direction);
void step_thrown_bolt(Game& game,int slot);
Cell thrown_bolt_next(const Entity& shot);
bool valid_thrown_bolt(const Entity& shot);
