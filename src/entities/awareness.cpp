#include "behavior.hpp"

#include <algorithm>

void remember_attacker(Game& game, int slot, Cell from) {
    Entity& victim = game.entities[static_cast<std::size_t>(slot)];
    if (victim.kind != EntityKind::Chicken) return;
    const int attacker = entity_at(game, from, true);
    const Handle threat = attacker < 0 || attacker == slot ? Handle{} :
        Handle{attacker, game.entities[static_cast<std::size_t>(attacker)].generation};
    // ALARM: Nearby flockmates remember the same threat even if this bird dies.
    for (Entity& bird : game.entities) {
        if (bird.kind != EntityKind::Chicken || distance(victim.cell, bird.cell) > 5) continue;
        bird.entity_b = threat;
        bird.point_b = from;
        bird.timer_a = 180;
    }
}

void flee(Game& game, int slot, Cell threat) {
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    if (entity.move_wait > 0) return;
    constexpr Cell sides[]{{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    Cell best = entity.cell;
    int score = distance(best, threat);
    const unsigned int offset = random_u32(game) % 4;
    for (unsigned int i = 0; i < 4; ++i) {
        const Cell cell = entity.cell + sides[(i + offset) % 4];
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile) || entity_at(game, cell, true) >= 0) continue;
        if (distance(cell, threat) > score) { best = cell; score = distance(cell, threat); }
    }
    if (best != entity.cell) move_entity(game, slot, best);
    else entity.move_wait = std::max(1, entity.move_interval);
}
