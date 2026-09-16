#pragma once
#include "../game.hpp"
// counter_a: ordinary/fisher/cub/mother/father/old solitary bear.
// entity_a: generation-qualified family identity, also the cub's living parent.
enum BearRole { BearOrdinary, BearFisher, BearCub, BearMother, BearFather, BearOld };
void set_bear_role(Entity& bear,BearRole role);
void alarm_bear_family(Game& game,int victim,Cell source);
bool step_bear_cub(Game& game,int slot);
int bear_reach(const Entity& bear);
int bear_damage(const Entity& bear);
float bear_size(const Entity& bear);
