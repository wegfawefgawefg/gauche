#pragma once
#include "../game.hpp"

struct DiverRoute { bool found = false; Cell hole{}, next{}; };
bool diver_pool(TileKind kind);
bool diver_hole_open(const Game& game, Cell cell, int diver_slot);
DiverRoute diver_route(const Game& game, int slot, Cell target);
