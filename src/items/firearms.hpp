#pragma once
#include "../game.hpp"

bool fire_weapon(Game& game, int user_slot, Cell direction, Item& item);
SoundId firearm_reload_sound(ItemKind kind);
