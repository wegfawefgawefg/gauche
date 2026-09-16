#pragma once
#include "../game.hpp"

struct FloorPlan;
bool gap_flyer(const Entity& actor);
bool grounded_projectile(const Entity& actor);
void remove_unsupported_body(Game& game,int slot,SoundId sound);
bool navigable_ground(const Entity& actor,const Tile& tile);
bool chasm_contact(Game& game,int slot);
bool finish_chasm_death(Game& game,int slot);
void step_chasm_contacts(Game& game);
void place_chasms(Game& game,const FloorPlan& plan);
