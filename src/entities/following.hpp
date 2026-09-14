#pragma once
#include "../game.hpp"

// TRAIL CONTRACT: point_b = departed cell, timer_b = delay before followers move.
// entity_a = preceding member. All participants must own these same slots.
void record_trail(Entity& leader, Cell previous, int delay);
void follow_trail(Game& game, int slot, const Entity& leader);
// YIELD CONTRACT: counter_c is a caller-decremented request lifetime on each member.
bool yield_trail(Game& game, int slot, const Entity& leader);
Handle find_chain_tail(const Game& game, int slot, EntityKind kind, int radius);
