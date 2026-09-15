#pragma once
#include "../game.hpp"
enum CranePhase { CraneIdle, CraneTravel, CraneLock, CraneReturn };
constexpr int crane_reach=5;
void init_magnet_crane(Entity& crane);
void step_magnet_crane(Game& game,int slot);
void interrupt_magnet_crane(Entity& crane);
bool valid_magnet_crane(const Entity& crane);
