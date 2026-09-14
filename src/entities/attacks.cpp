#include "attacks.hpp"
#include "../props/interaction.hpp"

#include <algorithm>
#include <cstdlib>

EnemyAttack enemy_attack(const Entity& enemy) {
    EnemyAttack attack;
    const auto add = [&attack](Cell cell) {
        if (attack.count < static_cast<int>(attack.cells.size()))
            attack.cells[static_cast<std::size_t>(attack.count++)] = cell;
    };
    switch (enemy.kind) {
    case EntityKind::Wasp:
        if (enemy.label_a == 1) add(enemy.point_a);
        break;
    case EntityKind::ForagerGoblin:
        if (enemy.label_b == 1) add({enemy.counter_a, enemy.counter_b});
        break;
    case EntityKind::RootTurret:
        if (enemy.label_a == 1)
            for (int i = 1; i <= enemy.counter_a; ++i)
                add(enemy.cell + Cell{enemy.facing.x*i, enemy.facing.y*i});
        break;
    case EntityKind::Woodpecker:
        if (enemy.label_a == 1 || enemy.label_a == 2)
            for (int i = 1; i <= enemy.counter_a; ++i)
                add(enemy.cell + Cell{enemy.facing.x*i, enemy.facing.y*i});
        break;
    case EntityKind::Owl: case EntityKind::Mosquito:
        if (enemy.label_a == 1 || enemy.label_a == 2) add(enemy.point_b);
        break;
    case EntityKind::Boar:
        if (enemy.label_a == 1 || enemy.label_a == 2)
            for (int i = 1; i <= enemy.counter_a; ++i)
                add(enemy.cell + Cell{enemy.facing.x * i, enemy.facing.y * i});
        break;
    case EntityKind::Bear:
        if (enemy.label_a == 1) {
            const Cell side{-enemy.facing.y, enemy.facing.x};
            for (int reach = 1; reach <= 2; ++reach)
                for (int lane = -1; lane <= 1; ++lane)
                    add(enemy.cell + Cell{enemy.facing.x * reach + side.x * lane,
                                          enemy.facing.y * reach + side.y * lane});
        }
        break;
    case EntityKind::SporeToad: case EntityKind::LanternMoth:
        if (enemy.label_a == 1) {
            const int radius = enemy.kind == EntityKind::SporeToad ? 2 : 1;
            for (int y = -radius; y <= radius; ++y)
                for (int x = -radius; x <= radius; ++x)
                    if (std::abs(x) + std::abs(y) <= radius) add(enemy.cell + Cell{x, y});
            attack.sleep = true;
        }
        break;
    case EntityKind::BrambleGuard: case EntityKind::Wolf: case EntityKind::CrateMimic:
        if (enemy.label_a == 1) add(enemy.point_b);
        break;
    case EntityKind::Bat: case EntityKind::FrostBat:
        if (enemy.label_a == 1 || enemy.label_a == 2)
            for (int i = 1; i <= enemy.counter_a; ++i)
                add(enemy.cell + Cell{enemy.facing.x * i, enemy.facing.y * i});
        break;
    default: break;
    }
    return attack;
}

bool clear_sight(const Game& game, Cell from, Cell to, bool smoke_blocks) {
    if (smoke_blocks && game.stage.at_or_border(from).surface.smoke_ticks >= 60) return false;
    // GRID RAY: Corner gaps must not leak dust or a creature's line of sight.
    const int dx = to.x - from.x, dy = to.y - from.y;
    const int nx = std::abs(dx), ny = std::abs(dy);
    const Cell sx{dx > 0 ? 1 : -1, 0}, sy{0, dy > 0 ? 1 : -1};
    int ix = 0, iy = 0;
    Cell cell = from;
    const auto open = [&game, smoke_blocks](Cell at) {
        const Tile* tile = game.stage.at(at);
        return tile != nullptr && walkable(*tile) && (!smoke_blocks || tile->surface.smoke_ticks < 60);
    };
    while (ix < nx || iy < ny) {
        const int horizontal = (1 + 2 * ix) * ny, vertical = (1 + 2 * iy) * nx;
        if (horizontal == vertical) {
            if (!open(cell + sx) || !open(cell + sy)) return false;
            cell = cell + sx + sy; ++ix; ++iy;
        } else if (horizontal < vertical) { cell = cell + sx; ++ix; }
        else { cell = cell + sy; ++iy; }
        if (!open(cell)) return false;
    }
    return true;
}

void resolve_enemy_attack(Game& game, int slot, int damage, SoundId sound, int sleep) {
    Entity& enemy = game.entities[static_cast<std::size_t>(slot)];
    const EnemyAttack attack = enemy_attack(enemy);
    emit_sound(game, sound, enemy.cell);
    enemy.use_flash = 10;
    for (int i = 0; i < attack.count; ++i) {
        const Cell cell = attack.cells[static_cast<std::size_t>(i)];
        if (!clear_sight(game, enemy.cell, cell, false)) continue;
        if (damage > 0) hit_prop(game, cell, damage, enemy.cell);
        for (int target_slot = 0; target_slot < max_entities; ++target_slot) {
            Entity& target = game.entities[static_cast<std::size_t>(target_slot)];
            if (target_slot == slot || target.health <= 0 || target.cell != cell) continue;
            if (damage > 0) damage_entity(game, target_slot, damage, enemy.cell);
            if (sleep > 0 && target.health > 0 && target.impassable && !target.hard_blocker)
                target.sleep_ticks = std::max(target.sleep_ticks, sleep);
        }
    }
}
