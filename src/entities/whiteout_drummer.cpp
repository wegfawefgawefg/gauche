#include "whiteout_drummer.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include "../surfaces/whiteout.hpp"

namespace {

// SLOTS: label_a phase; counter_a beat number; timer_a beat/recovery.
// point_a fixed stance; point_b fixed squall center. c-slots retain hearing.
void recover(Entity& drummer, int ticks) {
    drummer.label_a = DrummerRecover;
    drummer.timer_a = ticks;
    drummer.sprite = Sprite::DrummerRest;
}

void beat(Game& game, Entity& drummer) {
    const SoundId sounds[]{SoundId::DrumBeat1,SoundId::DrumBeat2,SoundId::DrumBeat3};
    emit_sound(game,sounds[drummer.counter_a-1],drummer.cell);
    drummer.sprite = drummer.counter_a == 2 ? Sprite::DrummerRight : Sprite::DrummerLeft;
    drummer.timer_a = 24;
}

} // namespace

void init_whiteout_drummer(Entity& drummer) {
    drummer.health = drummer.max_health = 44;
    drummer.move_interval = 20;
    drummer.impassable = true;
    drummer.sprite = Sprite::WhiteoutDrummer;
}

void interrupt_whiteout_drummer(Entity& drummer) {
    if (drummer.kind == EntityKind::WhiteoutDrummer && drummer.health > 0 &&
        drummer.label_a == DrummerBeats) recover(drummer,90);
}

void step_whiteout_drummer(Game& game, int slot) {
    Entity& drummer = game.entities[static_cast<std::size_t>(slot)];
    if (drummer.label_a == DrummerRecover) {
        if (drummer.timer_a == 0) {
            drummer.label_a = DrummerSeek;
            drummer.sprite = Sprite::WhiteoutDrummer;
        }
        return;
    }
    if (drummer.label_a == DrummerBeats) {
        if (drummer.cell != drummer.point_a) { recover(drummer,90); return; }
        if (drummer.timer_a > 0) return;
        if (++drummer.counter_a <= 3) { beat(game,drummer); return; }
        // COMMIT: Target movement never drags the marked squall across the room.
        if (clear_sight(game,drummer.cell,drummer.point_b) &&
            raise_whiteout(game,drummer.point_b,2,240))
            emit_sound(game,SoundId::WhiteoutRush,drummer.point_b);
        recover(drummer,180);
        return;
    }
    const auto target = enemy_target(game,drummer.cell,9);
    if (!target || !clear_attack_sight(game,drummer.cell,target->cell)) {
        if (!step_hearing(game,slot)) wander(game,slot);
        return;
    }
    if (distance(drummer.cell,target->cell) > 6) { pursue(game,slot,target->cell); return; }
    drummer.facing = cardinal_toward(drummer.cell,target->cell,drummer.facing);
    drummer.point_a = drummer.cell; drummer.point_b = target->cell;
    drummer.label_a = DrummerBeats; drummer.counter_a = 1;
    beat(game,drummer);
}
