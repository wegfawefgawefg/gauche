#pragma once
#include "../game.hpp"

// Successful arrivals burn immediately; stationary exposure pulses every 30 ticks.
// Flight and heat-native actors never make ground contact with molten rock.
void contact_lava(Game& game,int slot,bool arrived);
