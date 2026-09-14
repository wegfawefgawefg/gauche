#pragma once

#include "../game.hpp"

bool shove_in_front(Game& game, int user_slot, Cell direction);
bool shove_actor(Game& game, int target_slot, Cell direction, Cell source);
bool blocks_facing(const Entity& actor, Cell attacker);
