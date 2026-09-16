#pragma once
#include "ant.hpp"
enum AntLoadPhase { LoadIdle, LoadAlign, LoadWarn, LoadRest };
void init_ant_load(Entity& load);
void step_ant_load(Game& game,int slot);
void step_ant_puller(Game& game,int slot);
void interrupt_ant_load(Entity& load);
Handle ant_colony(const Game& game,const Entity& member);
// Wide path keeps the load and both flanking pullers on dry, passable ground.
std::vector<Cell> ant_load_route(const Game& game,Cell origin,Cell target,int own_slot=-1);
