#pragma once
#include "../game.hpp"
#include <array>

enum ChoirPhase { ChoirRest, ChoirHum, ChoirCough };
void init_tar_singer(Game& game,Entity& actor);
Handle spawn_tar_choir(Game& game,const std::array<Cell,3>& cells);
void interrupt_tar_singer(Entity& actor);
void step_tar_singer(Game& game,int slot);
void drop_tar_choir(Game& game,const Entity& actor);
bool valid_tar_singer(const Entity& actor);
