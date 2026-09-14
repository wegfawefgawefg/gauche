#pragma once
#include "catalog.hpp"

const RegionalItem* forest_displacement_item(ItemKind kind);
bool blow_hunting_horn(Game& game, int slot, Cell direction);
bool movable_by_tool(const Entity& actor);
