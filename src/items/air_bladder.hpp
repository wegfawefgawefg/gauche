#pragma once
#include "catalog.hpp"

const RegionalItem* air_bladder_item(ItemKind kind);
int floating_cargo_in_front(const Game& game, const Entity& user, Cell direction);
bool use_air_bladder(Game& game, int slot, Cell direction);
