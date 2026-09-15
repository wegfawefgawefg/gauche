#pragma once
#include "../game.hpp"

enum StokerPhase { StokerReady, StokerPack, StokerScoop, StokerStrike, StokerRest };
void interrupt_stoker(Entity& actor);
bool damp_stoker(Entity& actor);
bool stoker_hot(const Entity& actor);
int stoker_at(const Game& game,Cell cell);
bool feed_stoker(Game& game,Cell cell);
