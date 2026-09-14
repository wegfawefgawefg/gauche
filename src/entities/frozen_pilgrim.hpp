#pragma once
#include "../game.hpp"
#include <optional>

enum PilgrimPhase { PilgrimCold, PilgrimThaw, PilgrimHunt, PilgrimStrike, PilgrimRecover, PilgrimFreeze };
void init_frozen_pilgrim(Entity& pilgrim);
void step_frozen_pilgrim(Game& game, int slot);
void interrupt_frozen_pilgrim(Entity& pilgrim);
void chill_frozen_pilgrim(Entity& pilgrim);
bool pilgrim_crusted(const Entity& pilgrim);
std::optional<Cell> pilgrim_warm_step(const Game& game, int slot);
