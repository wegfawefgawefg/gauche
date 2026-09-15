#pragma once
#include "../game.hpp"

struct LiquidLoad { LiquidKind kind=LiquidKind::None;int amount=0; };
bool pumpable_liquid(LiquidKind kind);
LiquidLoad collect_spill(Game& game,Cell cell,LiquidKind accepted,int capacity);
int discharge_spill(Game& game,Cell cell,LiquidLoad load);
void splash_coolant(Game& game,Cell cell,int amount);
