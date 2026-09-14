#pragma once
#include "../game.hpp"

bool coverable_prop(const Prop& prop);
bool can_cover_optic(const Tile& tile);
bool cover_optic(Game& game, Cell cell);
bool uncover_optic(Game& game, int slot);
void remove_prop_cover(Game& game, Cell cell, bool burning);
