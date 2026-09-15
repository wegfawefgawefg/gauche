#pragma once

#include "../game.hpp"

// TELLS: Preview and resolution consume the same committed cells.
struct EnemyAttack {
    std::array<Cell, 64> cells{};
    int count = 0;
    bool sleep = false;
};
EnemyAttack enemy_attack(const Entity& enemy);
void resolve_enemy_attack(Game& game, int slot, int damage, SoundId sound, int sleep = 0);
bool clear_shot_sight(const Game& game, Cell from, Cell to, bool smoke_blocks = true);
bool clear_sight(const Game& game, Cell from, Cell to, bool smoke_blocks = true);
bool clear_attack_sight(const Game& game, Cell from, Cell to, bool smoke_blocks = true);
int enemy_defense(Game& game, int slot, int damage, Cell source, bool blockable);
