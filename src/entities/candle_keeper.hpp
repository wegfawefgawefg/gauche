#pragma once
#include "../game.hpp"
#include <optional>

enum KeeperPhase { KeeperTend, KeeperRelight, KeeperStrike, KeeperRecover };
void init_candle_keeper(Entity& keeper);
void step_candle_keeper(Game& game, int slot);
bool douse_keeper_lamp(Game& game, Entity& keeper);
void step_keeper_lamp(Game& game, int slot);
void interrupt_candle_keeper(Entity& keeper);
void keeper_candle_stolen(Game& game, Cell cell, Handle thief);
std::optional<Cell> keeper_candle(const Game& game, int slot);
void keeper_walk(Game& game, int slot, Cell target);
