#pragma once
#include "../game.hpp"
bool launch_coal_spit(Game& game,int owner,Cell direction,bool hot);
void step_coal_spit(Game& game,int slot);
bool douse_coal_spit(Entity& actor);
