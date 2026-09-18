#pragma once

#include "../game.hpp"

void place_coins(Game& game, Cell cell, int amount, bool player_drop = false);
void collect_coins(Game& game, Entity& player);
void drop_enemy_loot(Game& game, const Entity& enemy);
