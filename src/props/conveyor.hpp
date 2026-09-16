#pragma once
#include "../game.hpp"
#include <vector>

constexpr int belt_manual=4;
constexpr int belt_beat=20;
bool live_belt(const Prop& prop);
bool belt_powered(const Game& game,Cell cell);
Cell belt_direction(const Prop& prop);
std::vector<Cell> belt_run(const Game& game,Cell cell);
void step_belt_timer(Prop& prop);
void step_conveyors(Game& game);
bool crank_belt(Game& game,Cell cell);
bool brake_belt(Game& game,Cell cell);
void hit_belt_brake(Game& game,Cell cell,int damage);
