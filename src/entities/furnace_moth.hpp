#pragma once
#include "../game.hpp"
enum FurnaceMothPhase { FurnaceOrbit, FurnaceFeed, FurnaceWarn, FurnaceDive, FurnaceRest };
void init_furnace_moth(Entity& actor);
void step_furnace_moth(Game& game,int slot);
void interrupt_furnace_moth(Entity& actor);
bool cool_furnace_moth(Entity& actor);
bool valid_furnace_moth(const Entity& actor);
