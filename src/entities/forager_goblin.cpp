#include "dispatch.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "../surfaces/scent.hpp"
#include "attacks.hpp"
#include "scavenging.hpp"

#include <algorithm>

// SLOTS: point_a home/point_b patrol end; entity_a loose item; entity_b attacker;
// label_a patrol end; label_b knife tell; counter_a/b knife target; counter_c alarm;
// timer_a forage/rest, timer_b knife beat. Held slot contains actual stolen contents.
void init_forager_goblin(Game& game, Entity& goblin) {
    goblin.sprite = Sprite::ForagerGoblin;
    goblin.health = goblin.max_health = 34;
    goblin.move_interval = 14;
    goblin.impassable = true;
    goblin.point_a = goblin.point_b = goblin.cell;
    const unsigned int start = random_u32(game) % 4;
    constexpr Cell sides[]{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    for (unsigned int i = 0; i < 4; ++i) {
        const Cell side = sides[(i + start) % 4];
        Cell end = goblin.cell;
        for (int step = 0; step < 5; ++step) {
            const Cell next = end + side;
            const Tile* tile = game.stage.at(next);
            if (tile == nullptr || !walkable(*tile) || entity_at(game, next, true) >= 0) break;
            end = next;
        }
        if (distance(goblin.cell, end) > distance(goblin.cell, goblin.point_b)) goblin.point_b = end;
    }
}

namespace {

bool defend_loot(Game& game, int slot, Entity& goblin) {
    if (goblin.label_b == 1) {
        if (goblin.timer_b > 0) return true;
        const Cell target{goblin.counter_a, goblin.counter_b};
        if (distance(goblin.cell, target) == 1)
            resolve_enemy_attack(game, slot, 6, SoundId::KnifeStab);
        goblin.label_b = 0; goblin.timer_b = 45;
        goblin.sprite = Sprite::ForagerGoblin;
        return true;
    }
    int threat = nearest_player(game, goblin.cell, 3);
    if (goblin.counter_c > 0) {
        const Entity* attacker = get_entity(game, goblin.entity_b);
        if (attacker != nullptr && attacker->health > 0 && distance(goblin.cell, attacker->cell) <= 7)
            threat = goblin.entity_b.slot;
    }
    if (threat < 0) return false;
    const Cell from = game.entities[static_cast<std::size_t>(threat)].cell;
    if (goblin.move_wait > 0) return true;
    const Cell previous = goblin.cell;
    flee(game, slot, from);
    if (previous == goblin.cell && distance(goblin.cell, from) == 1 && goblin.timer_b == 0) {
        goblin.label_b = 1; goblin.timer_b = 22;
        goblin.counter_a = from.x; goblin.counter_b = from.y;
        goblin.facing = from - goblin.cell;
        goblin.sprite = Sprite::ForagerGoblinKnife;
        emit_sound(game, SoundId::GoblinHiss, goblin.cell);
    }
    return true;
}

} // namespace

void step_forager_goblin(Game& game, int slot) {
    Entity& goblin = game.entities[static_cast<std::size_t>(slot)];
    goblin.counter_c = std::max(0, goblin.counter_c - 1);
    const bool carrying = goblin.inventory.held()->count > 0;
    goblin.move_interval = carrying ? 8 : 14;
    if (defend_loot(game, slot, goblin)) return;
    if (carrying) {
        if (goblin.cell != goblin.point_a) pursue(game, slot, goblin.point_a);
        else if (goblin.timer_a == 0) { emit_sound(game, SoundId::GoblinChuckle, goblin.cell); goblin.timer_a = 360; }
        return;
    }
    if (step_hearing(game, slot)) return;
    if (step_scent(game, slot)) return;
    if (goblin.timer_a == 0) {
        goblin.entity_a = find_scavenge(game, slot, false, 7);
        goblin.timer_a = 45;
    }
    Entity* loose = get_entity(game, goblin.entity_a);
    if (loose != nullptr && loose->kind == EntityKind::GroundItem) {
        if (collect_scavenge(game, slot, goblin.entity_a, false)) { goblin.entity_a = {}; return; }
        pursue(game, slot, loose->cell);
        return;
    }
    const Cell end = goblin.label_a == 0 ? goblin.point_b : goblin.point_a;
    if (goblin.cell == end) {
        if (goblin.move_wait == 0) { goblin.label_a ^= 1; goblin.move_wait = 70; }
    } else pursue(game, slot, end);
}
