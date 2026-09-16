#pragma once
#include "../game.hpp"
// True when contact consumed or reflected the shot; stop its current step.
bool arrow_cell_contact(Game& game,int shot_slot);
// Walking, shoves, portals and other grounded arrivals share this contact path.
void contact_arrows(Game& game,int actor_slot);
