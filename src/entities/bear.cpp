#include "behavior.hpp"
#include "bear_fishing.hpp"
#include "hearing.hpp"
#include "foraging.hpp"
#include "dispatch.hpp"
#include "attacks.hpp"

// SLOTS: point_a territory, point_b remembered threat; label_a walk/rear/recover;
// timer_a attack beat; timer_b anger. A retreat beyond its territory ends pursuit.
void init_bear(Entity& bear) {
    bear.sprite = Sprite::Bear;
    bear.health = bear.max_health = 140;
    bear.move_interval = 28;
    bear.impassable = true;
    bear.point_a = bear.cell;
}

void step_bear(Game& game, int slot) {
    Entity& bear = game.entities[static_cast<std::size_t>(slot)];
    if (step_foraging(game, slot, bear.label_a != 0 || bear.timer_b > 0)) {
        if (bear.counter_a==1) bear.sprite=bear.label_b==1 ? Sprite::BearFishEat : Sprite::Bear;
        return;
    }
    if (bear.label_a == 1) {
        if (bear.timer_a > 0) return;
        resolve_enemy_attack(game, slot, 24, SoundId::BearSlam);
        bear.label_a = 2;
        bear.timer_a = 70;
        return;
    }
    if (bear.label_a == 2) {
        if (bear.timer_a == 0) bear.label_a = 0;
        return;
    }
    if (step_hearing(game, slot)) {interrupt_bear_fishing(bear);return;}
    if (step_bear_fishing(game,slot)) return;
    const auto target = enemy_target(game, bear.cell, bear.timer_b > 0 ? 8 : 3);
    if (!target || distance(bear.cell, bear.point_a) > 8) {
        if (distance(bear.cell, bear.point_a) > 1) pursue(game, slot, bear.point_a);
        else if (game.tick % 60 == 0) wander(game, slot);
        return;
    }
    const Cell cell = target->cell;
    if (!clear_attack_sight(game, bear.cell, cell)) return;
    bear.point_b = cell;
    bear.timer_b = 300;
    if (distance(bear.cell, cell) <= 2) {
        bear.facing = cardinal_toward(bear.cell, cell, bear.facing);
        bear.label_a = 1;
        bear.timer_a = 42;
        emit_sound(game, SoundId::BearRear, bear.cell);
    } else pursue(game, slot, cell);
}
