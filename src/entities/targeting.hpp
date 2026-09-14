#pragma once

#include "../game.hpp"
#include <optional>

struct EnemyTarget {
    Cell cell{};
    Handle actor{}; // Empty for a straw prop; committed attacks already store cells.
};
bool live_decoy(const Prop& prop);
std::optional<EnemyTarget> enemy_target(const Game& game, Cell from, int radius,
                                       bool include_players = true);
