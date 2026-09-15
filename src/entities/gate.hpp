#pragma once
#include "../game.hpp"

void request_gate(Game& game, Entity& gate, bool open);
void step_gate(Game& game, int slot);
void configure_timed_gate(Entity& gate);
bool valid_gate_state(const Entity& gate);
