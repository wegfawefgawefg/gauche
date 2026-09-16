#pragma once
#include "../game.hpp"

constexpr int reactor_deadline=3600;
constexpr int reactor_warning=90;
constexpr int reactor_budget=8;
bool make_last_shift(Game& game,bool force);
void populate_last_shift(Game& game);
const Entity* reactor_core(const Game& game);
bool activate_reactor(Game& game,Entity& core);
void step_reactor(Game& game);
bool exposed_reactor_cell(const Stage& stage,Cell cell);
bool valid_reactor(const Game& game);
