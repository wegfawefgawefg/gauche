#include "effects.hpp"
#include "../game.hpp"
#include "../surfaces/interaction.hpp"

#include <algorithm>

bool apply_sleep(Entity& actor, int ticks) {
    if (actor.health <= 0 || actor.vitals.sleep_guard > 0 || ticks <= 0) return false;
    actor.sleep_ticks = std::max(actor.sleep_ticks, ticks);
    return true;
}

bool apply_stun(Entity& actor, int ticks) {
    if (actor.health <= 0 || actor.vitals.stun_guard > 0 || ticks <= 0) return false;
    actor.stun_ticks = std::max(actor.stun_ticks, ticks);
    return true;
}

int movement_recovery_rate(const Entity& actor) {
    return actor.vitals.haste > 0 ? 2 : 1;
}

int movement_beat(const Entity& actor, int recovery) {
    const int rate = movement_recovery_rate(actor);
    return ((recovery + rate - 1) / rate) * (actor.freeze_ticks > 0 ? 2 : 1);
}

void step_vital_effects(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    VitalEffects& effects = actor.vitals;
    if (actor.health <= 0) { effects = {}; return; }
    if (effects.sleep_guard > 0) --effects.sleep_guard;
    if (effects.stun_guard > 0) --effects.stun_guard;

    // HERBS: Spend one point every third of a second, even if already fully healed.
    if (effects.healing_left > 0) {
        if (effects.healing_wait > 0) --effects.healing_wait;
        if (effects.healing_wait == 0) {
            actor.health = std::min(actor.max_health, actor.health + 1);
            --effects.healing_left;
            effects.healing_wait = effects.healing_left > 0 ? 20 : 0;
        }
    }

    // CHILI: The sprint ends in a short weak burn; standing water quenches it.
    if (effects.haste > 0 && --effects.haste == 0) {
        const Tile* ground = game.stage.at(actor.cell);
        if (ground == nullptr || !surface_wet(*ground)) {
            if (actor.scorch_ticks == 0) emit_sound(game, SoundId::FirePanic, actor.cell);
            actor.scorch_ticks = std::max(actor.scorch_ticks, 90);
        }
    }
}
