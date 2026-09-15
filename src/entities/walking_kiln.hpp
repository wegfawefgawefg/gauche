#pragma once
#include "../game.hpp"
enum KilnPhase { KilnWalk, KilnDoor, KilnBreath, KilnFeed, KilnCooled, KilnRecover };
void init_walking_kiln(Entity& actor);
void step_walking_kiln(Game& game,int slot);
void interrupt_walking_kiln(Entity& actor);
bool cool_walking_kiln(Entity& actor);
bool kiln_hot(const Entity& actor);
bool kiln_open(const Entity& actor);
bool valid_walking_kiln(const Entity& actor);
