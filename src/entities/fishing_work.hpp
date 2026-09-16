#pragma once
#include "../game.hpp"
void start_fishing_work(Entity& widow,Cell facing);
bool step_fishing_work(Game& game,int slot);
void rouse_fishing_worker(Game& game,Entity& widow);
bool valid_fishing_widow(const Entity& widow);
