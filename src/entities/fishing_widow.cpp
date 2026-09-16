#include "fishing_widow.hpp"
#include "fishing_work.hpp"
#include "attacks.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "../projectiles/widow_hook.hpp"

namespace {

// SLOTS: label_a phase; timer_a phase; counter_a cast reach; point_a stance origin.
// facing locks the casting lane; entity_a active hook. c-slots are hearing.
void hunt(Game& game, int slot) {
    Entity& widow = game.entities[static_cast<std::size_t>(slot)];
    const auto target = enemy_target(game, widow.cell, 10);
    if (!target || !clear_attack_sight(game, widow.cell, target->cell)) {
        if (!step_hearing(game, slot)) wander(game, slot);
        return;
    }
    const Cell cell = target->cell;
    if (distance(widow.cell, cell) <= widow.counter_a &&
        (widow.cell.x == cell.x || widow.cell.y == cell.y)) {
        widow.point_a = widow.cell;
        widow.facing = cardinal_toward(widow.cell, cell, widow.facing);
        widow.label_a = WidowWindup; widow.timer_a = 36;
        widow.sprite = Sprite::WidowWindup;
        emit_sound(game, SoundId::WidowWarn, widow.cell);
        return;
    }
    // LANES: Walk toward a visible target until a cardinal cast is possible.
    pursue(game, slot, cell);
}

} // namespace

void init_fishing_widow(Entity& widow) {
    widow.health = widow.max_health = 64;
    widow.move_interval = 21;
    widow.impassable = true;
    widow.counter_a = 6;
    widow.sprite = Sprite::FishingWidow;
}

void interrupt_fishing_widow(Entity& widow) {
    if (widow.kind != EntityKind::FishingWidow || widow.health <= 0) return;
    if (widow.label_b==1) {
        widow.label_a=WidowWorkWait;widow.timer_a=90;widow.entity_a={};
        widow.sprite=Sprite::WidowUntangle;return;
    }
    if (widow.label_a != WidowWindup && widow.label_a != WidowLine) return;
    widow.label_a = WidowUntangle; widow.timer_a = 90;
    widow.entity_a = {};
    widow.sprite = Sprite::WidowUntangle;
}

void step_fishing_widow(Game& game, int slot) {
    if (step_fishing_work(game,slot)) return;
    Entity& widow = game.entities[static_cast<std::size_t>(slot)];
    if (widow.label_a == WidowWindup || widow.label_a == WidowLine) {
        if (widow.cell != widow.point_a) { interrupt_fishing_widow(widow); return; }
    }
    switch (widow.label_a) {
    case WidowWindup:
        if (widow.timer_a > 0) return;
        widow.entity_a = launch_widow_hook(game, slot);
        if (get_entity(game, widow.entity_a)) {
            widow.label_a = WidowLine;
            widow.sprite = Sprite::WidowReel;
            emit_sound(game, SoundId::WidowCast, widow.cell);
        } else interrupt_fishing_widow(widow);
        return;
    case WidowLine:
        if (!get_entity(game, widow.entity_a)) interrupt_fishing_widow(widow);
        return;
    case WidowUntangle:
        if (widow.timer_a == 0) { widow.label_a = WidowHunt; widow.sprite = Sprite::FishingWidow; }
        return;
    default: hunt(game, slot); return;
    }
}
