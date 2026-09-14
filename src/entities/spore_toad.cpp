#include "behavior.hpp"
#include "hearing.hpp"
#include "dispatch.hpp"
#include "attacks.hpp"

// SLOTS: label_a resting/swelling/retreating; timer_a phase; point_a puff threat.
void init_spore_toad(Entity& toad) {
    toad.sprite = Sprite::SporeToad;
    toad.health = toad.max_health = 28;
    toad.move_interval = 20;
    toad.impassable = true;
}

void step_spore_toad(Game& game, int slot) {
    Entity& toad = game.entities[static_cast<std::size_t>(slot)];
    if (toad.label_a == 1) {
        if (toad.timer_a > 0) return;
        resolve_enemy_attack(game, slot, 0, SoundId::ToadPuff, 100);
        toad.sprite = Sprite::SporeToad;
        toad.label_a = 2;
        toad.timer_a = 100;
        return;
    }
    if (toad.label_a == 2) {
        flee(game, slot, toad.point_a);
        if (toad.timer_a == 0) toad.label_a = 0;
        return;
    }
    if (step_hearing(game, slot)) return;
    const int target = nearest_player(game, toad.cell, 3);
    if (target < 0) { if (game.tick % 30 == 0) wander(game, slot); return; }
    const Cell cell = game.entities[static_cast<std::size_t>(target)].cell;
    if (!clear_sight(game, toad.cell, cell)) return;
    toad.point_a = cell;
    toad.label_a = 1;
    toad.timer_a = 48;
    toad.sprite = Sprite::SporeToadSwollen;
    emit_sound(game, SoundId::ToadCroak, toad.cell);
}
