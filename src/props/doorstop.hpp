#pragma once
#include "../game.hpp"

bool doorstop_present(const Prop& prop);
bool place_doorstop(Game& game, Cell cell, const Item& item);
Item recoverable_doorstop(const Prop& prop);
int release_doorstop(Game& game, Cell cell, Cell destination);
