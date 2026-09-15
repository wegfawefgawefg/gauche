#pragma once
#include "../game.hpp"

bool circuit_prop(const Prop& prop);
bool grounding_ready(const Prop& prop);
bool absorb_grounded_shock(Game& game, Cell cell);
void cool_grounding_spike(Game& game, Cell cell);
void step_grounding_spike(Game& game, Cell cell);
Item recoverable_spike(const Prop& prop);
int release_grounding_spike(Game& game, Cell cell, Cell destination);
