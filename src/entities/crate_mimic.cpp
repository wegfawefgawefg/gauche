#include "behavior.hpp"
#include "crate_mimic.hpp"
#include "hearing.hpp"
#include "dispatch.hpp"
#include "attacks.hpp"

void interrupt_mimic_bite(Entity& mimic) {
    if (mimic.kind!=EntityKind::CrateMimic || mimic.label_a!=MimicWindup) return;
    mimic.label_a=MimicRecover;mimic.timer_a=mimic_recovery_ticks;
    mimic.sprite=Sprite::MimicRecover;mimic.use_flash=0;
}

bool valid_mimic(const Entity& mimic) {
    return mimic.kind!=EntityKind::CrateMimic || (mimic.label_a>=MimicHidden && mimic.label_a<=MimicRecover &&
        mimic.timer_a>=0 && mimic.timer_a<=45 &&
        (mimic.label_a!=MimicWindup || distance({},mimic.facing)==1));
}
void init_crate_mimic(Entity& mimic) {
    mimic.sprite = Sprite::Crate;
    mimic.health = mimic.max_health = 160;
    mimic.move_interval = 12;
    mimic.impassable = true;
    mimic.point_a = mimic.cell;
}

void step_crate_mimic(Game& game, int slot) {
    Entity& mimic = game.entities[static_cast<std::size_t>(slot)];
    if (mimic.label_a == MimicWindup) {
        if (mimic.cell+mimic.facing!=mimic.point_b || mimic.stun_ticks>0 ||
            mimic.sleep_ticks>0 || mimic.toss.ticks>0) {
            interrupt_mimic_bite(mimic);return;
        }
        mimic.sprite=mimic.timer_a>12 ? Sprite::MimicOpen : Sprite::MimicGape;
        if (mimic.timer_a > 0) return;
        resolve_enemy_attack(game, slot, 20, SoundId::MimicBite);
        mimic.label_a = MimicRecover;
        mimic.timer_a = mimic_recovery_ticks;
        mimic.sprite = Sprite::MimicSnap;
        return;
    }
    if (mimic.label_a == MimicRecover) {
        if (mimic.use_flash==0) mimic.sprite=Sprite::MimicRecover;
        if (mimic.timer_a==0) {
            mimic.label_a=MimicRetreat;mimic.timer_a=45;
            mimic.sprite=Sprite::CrateMimic;
        }
        return;
    }
    if (mimic.label_a == MimicRetreat) {
        flee(game, slot, mimic.point_b);
        if (mimic.timer_a == 0) mimic.label_a = MimicHunt;
        return;
    }
    if (mimic.timer_c > 0 && mimic.label_a == MimicHidden) {
        mimic.label_a = MimicHunt; mimic.sprite = Sprite::CrateMimic;
        mimic.timer_b = 240; mimic.move_wait = 30;
        emit_sound(game, SoundId::MimicWake, mimic.cell);
    }
    if (step_hearing(game, slot)) { mimic.timer_b = 240; return; }
    const auto target = enemy_target(game, mimic.cell, mimic.label_a == MimicHidden ? 2 : 7);
    const bool visible = target.has_value() && clear_attack_sight(game, mimic.cell,
        target->cell);
    if (mimic.label_a == MimicHidden) {
        if (!visible && (mimic.health == mimic.max_health || mimic.counter_a != 0)) return;
        mimic.sprite = Sprite::CrateMimic;
        mimic.label_a = MimicHunt;
        mimic.timer_b = 240;
        mimic.move_wait = 30;
        emit_sound(game, SoundId::MimicWake, mimic.cell);
    }
    if (!visible) {
        if (mimic.timer_b == 0) {
            if (mimic.cell == mimic.point_a) {
                mimic.label_a = MimicHidden;
                mimic.sprite = Sprite::Crate;
                // Damaged mimics keep watching instead of repeatedly playing the wake sound.
                mimic.counter_a = 1;
            } else pursue(game, slot, mimic.point_a);
        }
        return;
    }
    mimic.timer_b = 240;
    const Cell cell = target->cell;
    if (distance(mimic.cell, cell) == 1 && mimic.move_wait == 0) {
        mimic.point_b = cell;
        mimic.facing = cardinal_toward(mimic.cell, cell, mimic.facing);
        mimic.label_a = MimicWindup;
        mimic.timer_a = mimic_windup_ticks;
        mimic.sprite = Sprite::MimicOpen;
    } else pursue(game, slot, cell);
}
