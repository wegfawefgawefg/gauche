#pragma once
#include "../game.hpp"

struct FloorPlan;
bool gap_flyer(const Entity& actor);
bool navigable_ground(const Entity& actor,const Tile& tile);
bool chasm_contact(Game& game,int slot);
bool finish_chasm_death(Game& game,int slot);
void step_chasm_contacts(Game& game);
void place_chasms(Game& game,const FloorPlan& plan);
