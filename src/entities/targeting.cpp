#include "targeting.hpp"
#include "behavior.hpp"
#include "attacks.hpp"

#include <algorithm>

bool live_decoy(const Prop& prop) {
    return prop.kind == PropKind::StrawDecoy && prop.hp > 0 && !prop.broken;
}

// ACQUISITION: A visible dummy appears two cells closer than a player. Equal scores
// keep the player; equal dummies keep scan order. No target or RNG state is invented.
std::optional<EnemyTarget> enemy_target(const Game& game, Cell from, int radius,
                                       bool include_players) {
    std::optional<EnemyTarget> choice;
    int score = radius + 1;
    const int player = include_players ? nearest_player(game, from, radius) : -1;
    if (player >= 0) {
        const Entity& actor = game.entities[static_cast<std::size_t>(player)];
        choice = EnemyTarget{actor.cell, {player, actor.generation}};
        score = distance(from, actor.cell);
    }
    const int extent = std::min(radius, 7);
    for (int y = -extent; y <= extent; ++y)
        for (int x = -extent; x <= extent; ++x) {
            const Cell cell = from + Cell{x, y};
            const Tile* tile = game.stage.at(cell);
            if (tile == nullptr || !live_decoy(tile->prop)) continue;
            const int range = std::clamp(static_cast<int>(tile->prop.variant), 6, 7);
            const int gap = distance(from, cell);
            if (gap > radius || gap > range || gap - 2 >= score ||
                !clear_attack_sight(game, from, cell)) continue;
            score = gap - 2;
            choice = EnemyTarget{cell, {}};
        }
    return choice;
}
