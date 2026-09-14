#include "behavior.hpp"
#include "dispatch.hpp"
#include "attacks.hpp"

// SLOTS: label_a hiding/bite/retreat/hunt; timer_a phase; timer_b unseen;
// point_a home; point_b committed bite cell. Sprite is presentation, not state.
void init_crate_mimic(Entity& mimic) {
    mimic.sprite = Sprite::Crate;
    mimic.health = mimic.max_health = 55;
    mimic.move_interval = 15;
    mimic.impassable = true;
    mimic.point_a = mimic.cell;
}

void step_crate_mimic(Game& game, int slot) {
    Entity& mimic = game.entities[static_cast<std::size_t>(slot)];
    if (mimic.label_a == 1) {
        if (mimic.timer_a > 0) return;
        resolve_enemy_attack(game, slot, 20, SoundId::MimicBite);
        mimic.label_a = 2;
        mimic.timer_a = 45;
        return;
    }
    if (mimic.label_a == 2) {
        flee(game, slot, mimic.point_b);
        if (mimic.timer_a == 0) mimic.label_a = 3;
        return;
    }
    const int target = nearest_player(game, mimic.cell, mimic.label_a == 0 ? 2 : 7);
    const bool visible = target >= 0 && clear_sight(game, mimic.cell,
        game.entities[static_cast<std::size_t>(target)].cell);
    if (mimic.label_a == 0) {
        if (!visible && (mimic.health == mimic.max_health || mimic.counter_a != 0)) return;
        mimic.sprite = Sprite::CrateMimic;
        mimic.label_a = 3;
        mimic.timer_b = 240;
        mimic.move_wait = 30;
        emit_sound(game, SoundId::MimicWake, mimic.cell);
    }
    if (!visible) {
        if (mimic.timer_b == 0) {
            if (mimic.cell == mimic.point_a) {
                mimic.label_a = 0;
                mimic.sprite = Sprite::Crate;
                // Damaged mimics keep watching instead of repeatedly playing the wake sound.
                mimic.counter_a = 1;
            } else pursue(game, slot, mimic.point_a);
        }
        return;
    }
    mimic.timer_b = 240;
    const Cell cell = game.entities[static_cast<std::size_t>(target)].cell;
    if (distance(mimic.cell, cell) == 1 && mimic.move_wait == 0) {
        mimic.point_b = cell;
        mimic.facing = cardinal_toward(mimic.cell, cell, mimic.facing);
        mimic.label_a = 1;
        mimic.timer_a = 24;
    } else pursue(game, slot, cell);
}
