#include "dispatch.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "attacks.hpp"

#include <algorithm>

namespace {

void finish_dart(Entity& bug, bool fed) {
    bug.label_a = fed ? 3 : 4;
    bug.timer_a = fed ? 120 : 42;
    bug.move_interval = 6;
    bug.sprite = fed ? Sprite::MosquitoFed : Sprite::Mosquito;
}

void dart(Game& game, int slot) {
    Entity& bug = game.entities[static_cast<std::size_t>(slot)];
    if (bug.move_wait > 0) return;
    if (bug.cell == bug.point_b || bug.counter_a <= 0) { finish_dart(bug, false); return; }
    const Cell next = bug.cell + cardinal_toward(bug.cell, bug.point_b, bug.facing);
    const int victim_slot = entity_at(game, next, true);
    if (victim_slot >= 0) {
        Entity& victim = game.entities[static_cast<std::size_t>(victim_slot)];
        const int health = victim.health;
        damage_entity(game, victim_slot, 5, bug.cell);
        const int drained = std::max(0, health-victim.health);
        bug.health = std::min(bug.max_health, bug.health+drained);
        emit_sound(game, SoundId::MosquitoBite, bug.cell);
        finish_dart(bug, drained > 0);
        return;
    }
    if (!move_entity(game, slot, next)) finish_dart(bug, false);
    else --bug.counter_a;
}

} // namespace

// SLOTS: label_a seek/hover/dart/digest/miss; timer_a phase; counter_a dart budget;
// point_b committed target cell; entity_a threat remembered during digestion.
void init_mosquito(Entity& bug) {
    bug.sprite = Sprite::Mosquito;
    bug.health = bug.max_health = 12;
    bug.move_interval = 12;
    bug.impassable = true;
}

void step_mosquito(Game& game, int slot) {
    Entity& bug = game.entities[static_cast<std::size_t>(slot)];
    if (bug.label_a == 1) {
        if (bug.timer_a == 0) { bug.label_a = 2; bug.move_interval = 3; bug.move_wait = 0; }
        return;
    }
    if (bug.label_a == 2) { dart(game, slot); return; }
    if (bug.label_a == 3 || bug.label_a == 4) {
        if (bug.label_a == 3) {
            const Entity* threat = get_entity(game, bug.entity_a);
            const Cell from = threat != nullptr ? threat->cell : bug.point_b;
            if (distance(bug.cell, from) < 4) flee(game, slot, from);
        }
        if (bug.timer_a == 0) { bug.label_a = 0; bug.move_interval = 12; bug.sprite = Sprite::Mosquito; }
        return;
    }
    if (step_hearing(game, slot)) return;
    const int target = nearest_player(game, bug.cell, 6);
    if (target < 0) { if (game.tick % 45 == 0) wander(game, slot); return; }
    const Entity& victim = game.entities[static_cast<std::size_t>(target)];
    if (!clear_sight(game, bug.cell, victim.cell)) return;
    if (distance(bug.cell, victim.cell) > 3) { approach(game, slot, victim.cell); return; }
    bug.point_b = victim.cell;
    bug.entity_a = {target, victim.generation};
    bug.facing = cardinal_toward(bug.cell, victim.cell, bug.facing);
    bug.counter_a = 3;
    bug.label_a = 1; bug.timer_a = 18;
    emit_sound(game, SoundId::MosquitoBuzz, bug.cell);
}
