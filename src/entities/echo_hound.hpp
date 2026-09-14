#pragma once
#include "../game.hpp"

enum EchoPhase { EchoListen, EchoTrail, EchoBite, EchoRecover };
void init_echo_hound(Entity& hound);
void step_echo_hound(Game& game, int slot);
void interrupt_echo_hound(Entity& hound);
void hear_echo_hounds(Game& game, Cell origin, int radius);
void hear_world_action(Game& game, SoundId sound, Cell origin);
