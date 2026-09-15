#pragma once
#include "../game.hpp"
enum WelderPhase { WelderReady, WelderMask, WelderSweep, WelderRecover };
void init_arc_welder(Entity& actor);
void step_arc_welder(Game& game,int slot);
void interrupt_arc_welder(Entity& actor);
bool valid_arc_welder(const Entity& actor);
