#pragma once
#include "../game.hpp"

enum LeechPhase { LeechSeek, LeechLatch, LeechFeed, LeechSwell, LeechRest };
void init_steam_leech(Entity& leech);
void step_steam_leech(Game& game, int slot);
void release_steam_leech(Entity& leech, int ticks);
bool leech_source_cell(const Game& game, const Entity& leech, Cell& cell);
bool leech_attached(const Game& game, const Entity& leech);
std::vector<Cell> leech_drain_cells(const Game& game);
bool leech_drains_cell(const Game& game, Cell cell);
