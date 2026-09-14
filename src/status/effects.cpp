#include "effects.hpp"
#include "../entities/steam_leech.hpp"
#include "../game.hpp"
#include "../surfaces/interaction.hpp"

#include <algorithm>

// CHILL: A movement penalty, not an input lock. Flames and cold creatures resist it.
bool apply_chill(Entity& actor, int ticks) {
    if (actor.health <= 0 || actor.move_interval <= 0 || actor.hard_blocker || ticks <= 0 ||
        actor.burn_ticks > 0 || actor.scorch_ticks > 0 || actor.kind == EntityKind::Ember ||
        actor.kind == EntityKind::FrostBat) return false;
    if (actor.kind == EntityKind::SteamLeech) release_steam_leech(actor, ticks);
    actor.freeze_ticks = std::clamp(std::max(actor.freeze_ticks, ticks), 0, 600);
    return true;
}

bool apply_nausea(Entity& actor, int ticks) {
    if (actor.health <= 0 || actor.move_interval <= 0 || actor.hard_blocker ||
        actor.kind == EntityKind::Ember || ticks <= 0) return false;
    if (actor.vitals.nausea == 0) actor.vitals.nausea_wait = 60;
    actor.vitals.nausea = static_cast<std::uint16_t>(std::clamp(std::max(ticks, static_cast<int>(actor.vitals.nausea)), 0, 600));
    return true;
}

bool apply_sleep(Entity& actor, int ticks) {
    if (actor.health <= 0 || actor.vitals.sleep_guard > 0 || ticks <= 0) return false;
    if (actor.kind == EntityKind::SteamLeech) release_steam_leech(actor, 90);
    actor.sleep_ticks = std::max(actor.sleep_ticks, ticks);
    return true;
}

bool apply_root(Entity& actor, int ticks, RootKind kind) {
    if (actor.health <= 0 || actor.move_interval <= 0 || actor.hard_blocker || ticks <= 0) return false;
    if (ticks >= actor.vitals.rooted) actor.vitals.root_kind = kind;
    actor.vitals.rooted = static_cast<std::uint16_t>(std::clamp(std::max(ticks, static_cast<int>(actor.vitals.rooted)), 0, 600));
    return true;
}

bool apply_stun(Entity& actor, int ticks) {
    if (actor.health <= 0 || actor.vitals.stun_guard > 0 || ticks <= 0) return false;
    if (actor.kind == EntityKind::SteamLeech) release_steam_leech(actor, 90);
    actor.stun_ticks = std::max(actor.stun_ticks, ticks);
    return true;
}

int movement_slow_factor(const Entity& actor) {
    return (actor.freeze_ticks > 0 ? 2 : 1) * (actor.vitals.grip > 0 ? 2 : 1);
}

int movement_recovery_rate(const Entity& actor) {
    return actor.vitals.haste > 0 ? 2 : 1;
}

int movement_beat(const Entity& actor, int recovery) {
    const int rate = movement_recovery_rate(actor);
    return ((recovery + rate - 1) / rate) * movement_slow_factor(actor);
}

void step_vital_effects(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    VitalEffects& effects = actor.vitals;
    if (actor.health <= 0) { effects = {}; return; }
    if (effects.rooted > 0) --effects.rooted;
    if (effects.rooted == 0) effects.root_kind = RootKind::Rope;
    if (effects.grip > 0 && --effects.grip == 0) emit_sound(game, SoundId::BootsRelease, actor.cell);
    if (effects.sleep_guard > 0) --effects.sleep_guard;
    if (effects.stun_guard > 0) --effects.stun_guard;

    // ROT: Refresh duration without resetting the damage beat; water clears both.
    if (effects.nausea > 0) {
        --effects.nausea;
        if (effects.nausea_wait > 0) --effects.nausea_wait;
        if (effects.nausea_wait == 0) {
            effects.nausea_wait = 60;
            damage_entity(game, slot, 1, actor.cell, false);
            if (actor.health <= 0) return;
        }
        if (effects.nausea == 0) effects.nausea_wait = 0;
    }

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
