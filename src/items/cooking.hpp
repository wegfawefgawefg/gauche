#pragma once
#include "../game.hpp"

constexpr int cooking_ticks=45;
ItemKind cooked_item(ItemKind kind);
bool cooking_heat(const Game& game,Cell cell);
bool cooking_action(const Entity& actor);
bool use_cooking_fire(Game& game,Entity& player,Cell fire,bool held_use);
bool step_cooking_action(Game& game,int slot,const Input& input);
void step_ground_cooking(Game& game,int slot);
bool valid_cooking_state(const Entity& actor);
