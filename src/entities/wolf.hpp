#pragma once
#include "../game.hpp"

enum WolfPhase {WolfHunt, WolfBiteWindup, WolfRecover};
constexpr int wolf_windup_ticks=18, wolf_recovery_ticks=32;
void interrupt_wolf_bite(Entity& wolf);
bool valid_wolf_bite(const Entity& wolf);
