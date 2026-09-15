#pragma once
#include "../game.hpp"
enum ShunterPhase { ShunterWork, ShunterBell, ShunterSwing, ShunterRest };
void init_rail_shunter(Entity& actor);
void interrupt_rail_shunter(Entity& actor);
void step_rail_shunter(Game& game,int slot);
bool valid_rail_shunter(const Entity& actor);
