#pragma once
#include "../game.hpp"

// True only while a reachable seed pile occupies this bird's attention.
bool feed_on_bird_seed(Game& game, int slot);
