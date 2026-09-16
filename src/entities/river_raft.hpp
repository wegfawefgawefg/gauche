#pragma once
#include "../game.hpp"

void init_river_raft(Entity& raft);
void step_river_raft(Game& game,int slot);
void board_river_raft(Game& game,int player_slot);
const Entity* ridden_river_raft(const Game& game,const Entity& actor);
void clear_river_raft(Game& game,Entity& raft);
void step_raft_cargo(Game& game,Entity& cargo);
bool valid_river_raft(const Entity& raft);
