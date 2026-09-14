#pragma once
#include "projectile.hpp"

enum FishingPhase { FishingOut, FishingEmpty, FishingCargo };
constexpr int fishing_beat = 4;
bool launch_fishing_hook(Game& game, int owner_slot, const Item& item, Cell direction);
void step_fishing_hook(Game& game, int slot);
