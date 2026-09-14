#pragma once
#include "../game.hpp"

bool scavenger_food(ItemKind kind);
Handle find_scavenge(const Game& game, int slot, bool food_only, int radius);
bool collect_scavenge(Game& game, int slot, Handle ground, bool one);
void drop_scavenged_items(Game& game, const Entity& scavenger);
