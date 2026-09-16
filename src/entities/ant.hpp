#pragma once
#include "../game.hpp"
enum AntRole { AntWorker, AntPorter, AntCaptain };
enum AntPhase { AntWorking, AntWindup, AntRecovery, AntWhistling };
void init_ant(Entity& ant);
void set_ant_role(Entity& ant,AntRole role);
void init_ant_nest(Entity& nest);
void init_ant_sugar(Entity& sugar);
void step_ant(Game& game,int slot);
void step_ant_nest(Entity& nest);
void step_ant_sugar(Entity& sugar);
void ant_timers(Entity& ant);
void hurt_ant_colony(Game& game,int slot,Cell attacker);
float ant_size(const Entity& ant);
