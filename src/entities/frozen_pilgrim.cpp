#include "frozen_pilgrim.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "attacks.hpp"
#include "../surfaces/temperature.hpp"

#include <algorithm>

namespace {

// SLOTS: label_a phase; label_b thawed body; timer_a phase/recovery; timer_b retained warmth.
// counter_a failed heat-search delay; counter_c pending freeze cue.
// point_a committed stance origin; point_b strike cell. c-slots are hearing.
void cold(Entity& pilgrim, int pause) {
    pilgrim.label_a = PilgrimCold; pilgrim.label_b = 0;
    pilgrim.timer_a = pause; pilgrim.timer_b = 0;
    pilgrim.counter_a = 0;
    pilgrim.move_interval = 36;
    pilgrim.sprite = Sprite::FrozenPilgrim;
}

void begin_freeze(Entity& pilgrim) {
    pilgrim.label_a = PilgrimFreeze;
    pilgrim.timer_a = 36; pilgrim.timer_b = 0;
    pilgrim.counter_c = 1;
    pilgrim.move_interval = 36;
    pilgrim.sprite = Sprite::PilgrimFreezing;
}

void seek_warmth(Game& game, int slot) {
    Entity& pilgrim = game.entities[static_cast<std::size_t>(slot)];
    if (pilgrim.counter_a > 0) { --pilgrim.counter_a; return; }
    if (pilgrim.move_wait > 0) return;
    const auto next = pilgrim_warm_step(game, slot);
    if (!next) { pilgrim.counter_a = 30; return; }
    if (*next != pilgrim.cell) willing_step(game, slot, *next);
}

void hunt(Game& game, int slot) {
    Entity& pilgrim = game.entities[static_cast<std::size_t>(slot)];
    const auto target = enemy_target(game, pilgrim.cell, 9);
    if (!target || !clear_attack_sight(game, pilgrim.cell, target->cell)) {
        if (!step_hearing(game, slot)) seek_warmth(game, slot);
        return;
    }
    if (distance(pilgrim.cell, target->cell) != 1) { pursue(game, slot, target->cell); return; }
    pilgrim.point_a = pilgrim.cell; pilgrim.point_b = target->cell;
    pilgrim.label_a = PilgrimStrike; pilgrim.timer_a = 18;
    pilgrim.facing = cardinal_toward(pilgrim.cell, target->cell, pilgrim.facing);
    pilgrim.sprite = Sprite::PilgrimStrike;
    emit_sound(game, SoundId::PilgrimWarn, pilgrim.cell);
}

} // namespace

void init_frozen_pilgrim(Entity& pilgrim) {
    pilgrim.health = pilgrim.max_health = 48;
    pilgrim.impassable = true;
    cold(pilgrim, 0);
}

bool pilgrim_crusted(const Entity& pilgrim) {
    return pilgrim.kind == EntityKind::FrozenPilgrim && pilgrim.label_b == 0;
}

void interrupt_frozen_pilgrim(Entity& pilgrim) {
    if (pilgrim.kind != EntityKind::FrozenPilgrim || pilgrim.health <= 0) return;
    if (pilgrim.label_a == PilgrimThaw) cold(pilgrim, 30);
    else if (pilgrim.label_a == PilgrimStrike) {
        pilgrim.label_a = PilgrimRecover; pilgrim.timer_a = 30;
        pilgrim.sprite = Sprite::PilgrimThawed;
    }
}

void chill_frozen_pilgrim(Entity& pilgrim) {
    if (pilgrim.kind != EntityKind::FrozenPilgrim || pilgrim.health <= 0) return;
    if (pilgrim.label_b != 0 && pilgrim.label_a != PilgrimFreeze) begin_freeze(pilgrim);
    else interrupt_frozen_pilgrim(pilgrim);
}

void step_frozen_pilgrim(Game& game, int slot) {
    Entity& pilgrim = game.entities[static_cast<std::size_t>(slot)];
    const bool warm = warm_cell(game, pilgrim.cell);
    // MOMENTUM: Four seconds away from warmth; no health reset on either transformation.
    if (pilgrim.label_b != 0 && pilgrim.label_a != PilgrimFreeze) {
        if (warm && pilgrim.freeze_ticks == 0) pilgrim.timer_b = 240;
        if (pilgrim.timer_b == 0 || pilgrim.freeze_ticks > 0) begin_freeze(pilgrim);
    }
    if (pilgrim.counter_c != 0) {
        pilgrim.counter_c = 0;
        emit_sound(game, SoundId::PilgrimFreeze, pilgrim.cell);
    }
    switch (pilgrim.label_a) {
    case PilgrimCold:
        if (pilgrim.timer_a > 0) return;
        if (warm && pilgrim.freeze_ticks == 0) {
            pilgrim.label_a = PilgrimThaw; pilgrim.timer_a = 48;
            pilgrim.point_a = pilgrim.cell;
            pilgrim.sprite = Sprite::PilgrimThawing;
            emit_sound(game, SoundId::PilgrimThaw, pilgrim.cell);
        } else seek_warmth(game, slot);
        return;
    case PilgrimThaw:
        if (pilgrim.cell != pilgrim.point_a || !warm || pilgrim.freeze_ticks > 0) {
            cold(pilgrim, 30); return;
        }
        if (pilgrim.timer_a == 0) {
            pilgrim.label_a = PilgrimHunt; pilgrim.label_b = 1; pilgrim.timer_b = 240;
            pilgrim.move_interval = 8; pilgrim.move_wait = std::min(pilgrim.move_wait, 8);
            pilgrim.sprite = Sprite::PilgrimThawed;
            emit_sound(game, SoundId::PilgrimWake, pilgrim.cell);
        }
        return;
    case PilgrimFreeze:
        if (pilgrim.timer_a == 0) cold(pilgrim, 12);
        return;
    case PilgrimStrike:
        if (pilgrim.cell != pilgrim.point_a) { interrupt_frozen_pilgrim(pilgrim); return; }
        if (pilgrim.timer_a == 0) {
            resolve_enemy_attack(game, slot, 16, SoundId::PilgrimSlash);
            interrupt_frozen_pilgrim(pilgrim);
        }
        return;
    case PilgrimRecover:
        if (pilgrim.timer_a == 0) pilgrim.label_a = PilgrimHunt;
        return;
    default: hunt(game, slot); return;
    }
}
