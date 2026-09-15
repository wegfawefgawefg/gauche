#pragma once
#include "../game.hpp"

enum class GroundAction { None, Pickup, Swap, Drop, Blocked };
bool item_can_drop(const Item& item);
Item pickup_item_at(const Game& game, Cell cell);
Item reachable_pickup_item(const Game& game, const Entity& player);
GroundAction ground_action(const Game& game, const Entity& player);
bool pickup_or_drop(Game& game, Entity& player);
