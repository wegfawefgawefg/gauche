#pragma once
#include "../game.hpp"

enum OperatorPhase { OperatorWorking, OperatorWaiting, OperatorFleeing, OperatorReturning };
void init_crane_operator(Entity& actor);
Handle staff_crane(Game& game,Handle crane,Cell station);
void step_crane_operator(Game& game,int slot);
void hurt_crane_operator(Game& game,Entity& victim,Cell source);
bool crane_operator_ready(const Game& game,const Entity& crane);
bool valid_crane_operator(const Entity& actor);
