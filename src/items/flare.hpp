#pragma once
#include "catalog.hpp"
const RegionalItem* flare_item(ItemKind kind);
bool launch_flare(Game& game,int owner,const Item& item,Cell direction);
bool burning_flare(const Entity& actor);
bool douse_flare(Game& game,Entity& actor);
void step_flare(Game& game,int slot);
bool valid_flare_state(const Entity& actor);
