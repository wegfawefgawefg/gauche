#include "behavior.hpp"
#include "foraging.hpp"

#include <algorithm>

void remember_attacker(Game& game, int slot, Cell from) {
    Entity& victim = game.entities[static_cast<std::size_t>(slot)];
    victim.timer_c = victim.label_c = 0; // Real harm ends a noise distraction.
    if (eats_meat(victim.kind)) { victim.counter_b = 360; victim.label_b = 0; }
    if (victim.kind == EntityKind::Bear) { victim.timer_b = 300; victim.point_b = from; }
    if (victim.kind == EntityKind::ForagerGoblin) {
        const int attacker = entity_at(game, from, true);
        if (attacker >= 0 && attacker != slot)
            victim.entity_b = {attacker, game.entities[static_cast<std::size_t>(attacker)].generation};
        victim.counter_c = 180;
    }
    if (victim.kind == EntityKind::Wasp || victim.kind == EntityKind::WaspNest) {
        for (Entity& bug : game.entities) {
            if (bug.kind != EntityKind::Wasp || bug.health <= 0 || distance(victim.cell, bug.cell) > 7) continue;
            // Never retarget a sting which is already winding up.
            if (bug.label_a != 1) bug.point_a = from;
            bug.counter_b = 300;
        }
    }
    if (victim.kind == EntityKind::CarrionCrow) {
        const int attacker = entity_at(game, from, true);
        if (attacker >= 0 && attacker != slot)
            victim.entity_b = {attacker, game.entities[static_cast<std::size_t>(attacker)].generation};
        victim.label_a = 3; victim.timer_a = 90;
        victim.sprite = Sprite::CarrionCrow;
    }
    if (victim.kind != EntityKind::Chicken) return;
    const int attacker = entity_at(game, from, true);
    const Handle threat = attacker < 0 || attacker == slot ? Handle{} :
        Handle{attacker, game.entities[static_cast<std::size_t>(attacker)].generation};
    // ALARM: Nearby flockmates remember the same threat even if this bird dies.
    for (Entity& bird : game.entities) {
        if (bird.kind != EntityKind::Chicken || distance(victim.cell, bird.cell) > 5) continue;
        bird.entity_b = threat;
        bird.counter_a = from.x;
        bird.counter_b = from.y;
        bird.timer_a = 480;
        bird.move_wait = std::min(bird.move_wait, 3);
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
