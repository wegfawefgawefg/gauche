#pragma once
#include "../game.hpp"
enum CutterState { CutterWorking, CutterBlocked, CutterExhausted };
void init_coal_cutter(Entity& cutter);
void step_coal_cutter(Game& game,int slot);
bool valid_coal_cutter(const Entity& cutter);
