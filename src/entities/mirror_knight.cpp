#include "mirror_knight.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "attacks.hpp"

namespace {

// SLOTS: label_a phase; timer_a phase; timer_b guard recovery.
// point_a committed stance origin; point_b committed sword cell. c-slots are hearing.
void rest(Entity& knight, int ticks) {
    knight.label_a = KnightRecover;
    knight.timer_a = ticks;
    knight.timer_b = 120;
    knight.sprite = Sprite::KnightRecover;
}

void begin_swing(Game& game, Entity& knight, Cell target) {
    knight.point_a = knight.cell;
    knight.point_b = target;
    knight.facing = cardinal_toward(knight.cell, target, knight.facing);
    knight.label_a = KnightSwing;
    knight.timer_a = 27;
    knight.sprite = Sprite::KnightSwing;
    emit_sound(game, SoundId::KnightWarn, knight.cell);
}

void advance(Game& game, int slot) {
    Entity& knight = game.entities[static_cast<std::size_t>(slot)];
    const auto target = enemy_target(game, knight.cell, 10);
    if (!target || !clear_attack_sight(game, knight.cell, target->cell)) {
        if (!step_hearing(game, slot)) wander(game, slot);
        return;
    }
    const int gap = distance(knight.cell, target->cell);
    if (gap == 1) { begin_swing(game, knight, target->cell); return; }
    // GUARD: A visible raise precedes reflection. It never tracks a flanking player.
    const bool aligned = knight.cell.x == target->cell.x || knight.cell.y == target->cell.y;
    if (gap <= 6 && aligned && knight.timer_b == 0) {
        knight.facing = cardinal_toward(knight.cell, target->cell, knight.facing);
        knight.point_a = knight.cell;
        knight.label_a = KnightRaise;
        knight.timer_a = 18;
        knight.sprite = Sprite::KnightRaise;
        emit_sound(game, SoundId::KnightRaise, knight.cell);
        return;
    }
    pursue(game, slot, target->cell);
}

} // namespace

void init_mirror_knight(Entity& knight) {
    knight.health = knight.max_health = 90;
    knight.move_interval = 22;
    knight.impassable = true;
    knight.sprite = Sprite::MirrorKnight;
}

bool knight_reflects(const Entity& knight, Cell incoming) {
    return knight.kind == EntityKind::MirrorKnight && knight.health > 0 &&
        knight.label_a == KnightGuard && knight.timer_a > 0 && knight.cell == knight.point_a &&
        knight.sleep_ticks == 0 && knight.stun_ticks == 0 &&
        knight.facing == Cell{-incoming.x, -incoming.y};
}

void interrupt_mirror_knight(Entity& knight) {
    if (knight.kind == EntityKind::MirrorKnight && knight.health > 0 &&
        (knight.label_a == KnightRaise || knight.label_a == KnightGuard || knight.label_a == KnightSwing))
        rest(knight, 48);
}

void step_mirror_knight(Game& game, int slot) {
    Entity& knight = game.entities[static_cast<std::size_t>(slot)];
    // DISPLACEMENT: A shove or swap cancels the old stance before it can strike.
    if (knight.cell != knight.point_a) interrupt_mirror_knight(knight);
    switch (knight.label_a) {
    case KnightRaise:
        if (knight.timer_a == 0) {
            knight.label_a = KnightGuard;
            knight.timer_a = 60;
            knight.sprite = Sprite::KnightGuard;
            emit_sound(game, SoundId::KnightReady, knight.cell);
        }
        return;
    case KnightGuard: {
        const auto target = enemy_target(game, knight.cell, 1);
        if (target && target->cell == knight.cell + knight.facing &&
            clear_attack_sight(game, knight.cell, target->cell)) begin_swing(game, knight, target->cell);
        else if (knight.timer_a == 0) rest(knight, 24);
        return;
    }
    case KnightSwing:
        if (knight.timer_a == 0) {
            resolve_enemy_attack(game, slot, 22, SoundId::KnightSlash);
            rest(knight, 48);
        }
        return;
    case KnightRecover:
        if (knight.timer_a == 0) { knight.label_a = KnightAdvance; knight.sprite = Sprite::MirrorKnight; }
        return;
    default: advance(game, slot); return;
    }
}
