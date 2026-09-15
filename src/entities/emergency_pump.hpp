#pragma once
#include "../game.hpp"
enum PumpPhase { PumpRoam, PumpFill, PumpWarn, PumpRecover };
void init_emergency_pump(Entity& actor);
void step_emergency_pump(Game& game,int slot);
void interrupt_emergency_pump(Entity& actor);
bool valid_emergency_pump(const Entity& actor);
void drop_emergency_pump(Game& game,const Entity& actor);
