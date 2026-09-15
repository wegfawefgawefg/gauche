#pragma once
#include "projectile.hpp"
enum ChainPhase { ChainOut, ChainProp, ChainCargo, ChainWall, ChainFixedProp, ChainFixture };
bool launch_chain_hook(Game& game,int owner,Cell direction);
void release_chain_hook(Game& game,Handle handle);
void step_chain_hook(Game& game,int slot);
bool valid_chain_hook(const Entity& actor);
