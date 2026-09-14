#pragma once
#include "../game.hpp"
#include <optional>

enum SealPhase { SealForage, SealRetreat, SealEat, SealBark, SealRecover };

bool seal_food(ItemKind kind);
bool seal_bank(const Game& game, Cell cell);
struct SealRoute { bool found = false; Cell next{}, end{}; Handle food{}; };
SealRoute seal_route(const Game& game, int slot, bool food, std::optional<Cell> threat = {});
void init_seal_thief(Entity& seal);
void interrupt_seal_thief(Entity& seal);
void step_seal_thief(Game& game, int slot);
