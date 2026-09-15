#pragma once
#include "catalog.hpp"
const RegionalItem* nozzle_elbow_item(ItemKind kind);
bool fit_nozzle_elbow(Game& game,int slot,Cell direction);
bool reverse_nozzle_elbow(Game& game,int slot);
Cell outlet_direction(const Entity& machine,Cell straight);
Item removable_machine_fitting(const Game& game,Cell cell);
int release_machine_fitting(Game& game,Cell source,Cell destination);
void damage_machine_fitting(Game& game,Entity& machine,int damage);
void drop_machine_fitting(Game& game,const Entity& machine);
bool valid_nozzle_elbow(const Item& item);
bool valid_machine_fitting(const Entity& machine);
