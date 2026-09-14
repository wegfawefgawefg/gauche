#pragma once
#include "../game.hpp"

enum DrummerPhase { DrummerSeek, DrummerBeats, DrummerRecover };
void init_whiteout_drummer(Entity& drummer);
void interrupt_whiteout_drummer(Entity& drummer);
void step_whiteout_drummer(Game& game, int slot);
