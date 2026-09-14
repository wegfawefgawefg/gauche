#include "dispatch.hpp"
#include "behavior.hpp"
#include "attacks.hpp"

#include <optional>

namespace {

bool protectable(EntityKind kind) {
    return kind == EntityKind::RootTurret || kind == EntityKind::SporeToad ||
        kind == EntityKind::LanternMoth || kind == EntityKind::ThornSnail || kind == EntityKind::Bear;
}

void choose_ward(Game& game, Entity& guard) {
    guard.entity_a = {};
    int best = 100;
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& ally = game.entities[static_cast<std::size_t>(slot)];
        if (!protectable(ally.kind) || ally.health <= 0 || distance(guard.point_a, ally.cell) > 8) continue;
        int score = distance(guard.cell, ally.cell);
        const Handle handle{slot, ally.generation};
        for (const Entity& other : game.entities)
            if (&other != &guard && other.kind == EntityKind::BrambleGuard &&
                other.health > 0 && other.entity_a == handle) score += 4;
        if (score < best) { best = score; guard.entity_a = handle; }
    }
    guard.timer_b = 60;
}

std::optional<Cell> screen_cell(const Game& game, const Entity& guard,
                               const Entity& ally, Cell threat) {
    const EnemyAttack attack = enemy_attack(ally);
    std::optional<Cell> choice;
    int best = 10000;
    for (Cell side : {Cell{1, 0}, {0, 1}, {-1, 0}, {0, -1}}) {
        const Cell cell = ally.cell + side;
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile) ||
            (cell != guard.cell && entity_at(game, cell, true) >= 0)) continue;
        bool threatened = false;
        for (int i = 0; i < attack.count; ++i)
            threatened |= cell == attack.cells[static_cast<std::size_t>(i)];
        if (threatened) continue;
        const int score = distance(cell, threat)*4 + distance(cell, guard.cell);
        if (score < best) { choice = cell; best = score; }
    }
    return choice;
}

} // namespace

// SLOTS: entity_a generation-checked ward; point_a territory; point_b committed
// strike; label_a guarding/swing/recovery; timer_a phase, timer_b ward search.
void init_bramble_guard(Entity& guard) {
    guard.sprite = Sprite::BrambleGuard;
    guard.health = guard.max_health = 90;
    guard.move_interval = 16;
    guard.impassable = true;
    guard.point_a = guard.cell;
    insert_item(guard.inventory, make_item(ItemKind::Buckler));
}

void step_bramble_guard(Game& game, int slot) {
    Entity& guard = game.entities[static_cast<std::size_t>(slot)];
    if (guard.label_a == 1) {
        if (guard.timer_a > 0) return;
        resolve_enemy_attack(game, slot, 16, SoundId::BrambleStrike);
        guard.sprite = Sprite::BrambleGuard;
        guard.label_a = 2; guard.timer_a = 42;
        return;
    }
    if (guard.label_a == 2) {
        if (guard.timer_a == 0) guard.label_a = 0;
        return;
    }
    const Entity* ally = get_entity(game, guard.entity_a);
    if ((ally == nullptr || ally->health <= 0) && guard.timer_b == 0) {
        choose_ward(game, guard);
        ally = get_entity(game, guard.entity_a);
    }
    const auto target = enemy_target(game, guard.cell, 7);
    if (!target || distance(guard.cell, guard.point_a) > 10) {
        const Cell home = ally != nullptr && ally->health > 0 ? ally->cell : guard.point_a;
        if (distance(guard.cell, home) > 1) pursue(game, slot, home);
        return;
    }
    const Cell threat = target->cell;
    if (!clear_attack_sight(game, guard.cell, threat)) return;
    guard.facing = cardinal_toward(guard.cell, threat, guard.facing);
    if (distance(guard.cell, threat) == 1) {
        guard.block_ticks = 0;
        guard.point_b = threat;
        guard.sprite = Sprite::BrambleGuardSwing;
        guard.label_a = 1; guard.timer_a = 24;
        emit_sound(game, SoundId::BrambleRustle, guard.cell);
        return;
    }
    // SCREEN: Physically intercept shots, but vacate the ward's committed attack.
    if (ally != nullptr && ally->health > 0) {
        if (const auto cell = screen_cell(game, guard, *ally, threat)) pursue(game, slot, *cell);
    } else if (distance(guard.cell, guard.point_a) > 1) pursue(game, slot, guard.point_a);
    guard.facing = cardinal_toward(guard.cell, threat, guard.facing);
    if (guard.inventory.held()->kind == ItemKind::Buckler) {
        guard.guard_slot = guard.inventory.selected;
        guard.block_ticks = 2;
    }
}
