#include "dispatch.hpp"
#include "behavior.hpp"
#include "attacks.hpp"

// SLOTS: label_a tracking/coiling/recovering; timer_a phase; timer_b turn delay.
// counter_a committed reach. Roots never pursue; a sidestep defeats the lash.
void init_root_turret(Entity& root) {
    root.sprite = Sprite::RootTurret;
    root.health = root.max_health = 65;
    root.impassable = root.hard_blocker = true;
    root.counter_a = 5;
}

void step_root_turret(Game& game, int slot) {
    Entity& root = game.entities[static_cast<std::size_t>(slot)];
    if (root.label_a == 1) {
        if (root.timer_a > 0) return;
        resolve_enemy_attack(game, slot, 18, SoundId::RootLash);
        root.label_a = 2;
        root.timer_a = 90;
        root.sprite = Sprite::RootTurret;
        return;
    }
    if (root.label_a == 2) {
        if (root.timer_a == 0) root.label_a = 0;
        return;
    }
    const auto target = enemy_target(game, root.cell, 6);
    if (!target) return;
    const Cell cell = target->cell;
    if (!clear_attack_sight(game, root.cell, cell)) return;
    const Cell desired = cardinal_toward(root.cell, cell, root.facing);
    if (root.facing != desired) {
        if (root.timer_b > 0) return;
        // TURN: Even a target behind it gets time to cross before the second quarter turn.
        const Cell clockwise{-root.facing.y, root.facing.x};
        root.facing = desired == Cell{-root.facing.x, -root.facing.y} ? clockwise : desired;
        root.timer_b = 18;
        return;
    }
    if (root.timer_b > 0 || distance(root.cell, cell) > root.counter_a ||
        (cell.x != root.cell.x && cell.y != root.cell.y)) return;
    root.label_a = 1;
    root.timer_a = 42;
    root.sprite = Sprite::RootTurretCoiled;
    emit_sound(game, SoundId::RootCoil, root.cell);
}
