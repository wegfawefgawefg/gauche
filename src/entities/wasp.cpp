#include "dispatch.hpp"
#include "behavior.hpp"
#include "following.hpp"
#include "attacks.hpp"

#include <algorithm>

// SLOTS: entity_a preceding wasp/nest, entity_b nest; point_a committed sting/threat;
// point_b/timer_b trail; label_a patrol/tell/recovery/scatter; timer_a phase;
// counter_a orphan latch; counter_b alarm memory; counter_c trail-yield request.
void init_wasp(Entity& bug) {
    bug.sprite = Sprite::Wasp;
    bug.health = bug.max_health = 16;
    bug.move_interval = 10;
    bug.impassable = true;
    bug.point_a = bug.point_b = bug.cell;
}

namespace {

bool feed_honey(Game& game, int slot, Entity& bug) {
    if (bug.counter_b > 0 || bug.move_wait > 0) return false;
    int nearest = -1, best = 7;
    for (int i = 0; i < max_entities; ++i) {
        const Entity& bait = game.entities[static_cast<std::size_t>(i)];
        if (bait.kind != EntityKind::GroundItem || bait.ground_item.kind != ItemKind::HoneyPot ||
            bait.ground_item.count <= 0) continue;
        const int gap = distance(bug.cell, bait.cell);
        const int occupant = entity_at(game, bait.cell, true);
        if (gap >= best || (occupant >= 0 && occupant != slot)) continue;
        if (gap > 0 && !next_route_cell(game, slot, bait.cell, 192)) continue;
        nearest = i; best = gap;
    }
    if (nearest < 0) return false;
    Entity& bait = game.entities[static_cast<std::size_t>(nearest)];
    if (best == 0) {
        if (--bait.ground_item.count == 0) remove_entity(game, {nearest, bait.generation});
        bug.label_a = 2; bug.timer_a = 180;
        emit_sound(game, SoundId::ScavengeTake, bug.cell);
    } else pursue(game, slot, bait.cell);
    return true;
}

void step_brood(Game& game, int slot, Entity& bug, const Entity* nest) {
    if (bug.label_a == 3) {
        flee(game, slot, bug.point_a);
        if (bug.timer_a == 0) { bug.label_a = 0; bug.move_interval = 10; }
        return;
    }
    if (bug.label_a == 1) {
        if (bug.timer_a > 0) return;
        if (distance(bug.cell, bug.point_a) == 1)
            resolve_enemy_attack(game, slot, 8, SoundId::WaspSting);
        bug.label_a = 2; bug.timer_a = 36; bug.sprite = Sprite::Wasp;
        return;
    }
    if (bug.label_a == 2) {
        if (bug.timer_a == 0) bug.label_a = 0;
        return;
    }
    if (feed_honey(game, slot, bug)) return;
    const int target = nearest_player(game, bug.cell, nest != nullptr ? 6 : 2);
    if (target >= 0) {
        const Entity& player = game.entities[static_cast<std::size_t>(target)];
        if (clear_sight(game, bug.cell, player.cell)) {
            bug.point_a = player.cell;
            bug.counter_b = 180;
            if (distance(bug.cell, player.cell) == 1) {
                bug.facing = player.cell - bug.cell;
                bug.label_a = 1; bug.timer_a = 18 + slot % 3 * 4;
                bug.sprite = Sprite::WaspSting;
                emit_sound(game, SoundId::WaspWarning, bug.cell);
            } else pursue(game, slot, player.cell);
            return;
        }
    }
    if (bug.counter_b > 0 && distance(bug.cell, bug.point_a) > 1) {
        pursue(game, slot, bug.point_a);
        return;
    }
    const Entity* leader = get_entity(game, bug.entity_a);
    if (leader == nullptr || leader->health <= 0) {
        bug.entity_a = bug.entity_b;
        leader = nest;
    }
    if (leader != nullptr && leader->kind == EntityKind::Wasp) {
        if (!yield_trail(game, slot, *leader)) follow_trail(game, slot, *leader);
    } else if (nest != nullptr && distance(bug.cell, nest->cell) > 3) pursue(game, slot, nest->cell);
    else wander(game, slot);
}

} // namespace

void step_wasp(Game& game, int slot) {
    Entity& bug = game.entities[static_cast<std::size_t>(slot)];
    bug.counter_b = std::max(0, bug.counter_b - 1);
    bug.counter_c = std::max(0, bug.counter_c - 1);
    const Entity* nest = get_entity(game, bug.entity_b);
    if (nest != nullptr && (nest->kind != EntityKind::WaspNest || nest->health <= 0)) nest = nullptr;
    if (nest == nullptr && bug.counter_a == 0) {
        // ORPHANS: Lose the attack, scatter from the former nest, then become local nuisances.
        bug.counter_a = 1; bug.counter_b = 0;
        bug.label_a = 3; bug.timer_a = 120; bug.move_interval = 6;
        bug.sprite = Sprite::Wasp;
        bug.point_a = bug.cell;
    }
    const Cell previous = bug.cell;
    step_brood(game, slot, bug, nest);
    record_trail(bug, previous, 4);
}
