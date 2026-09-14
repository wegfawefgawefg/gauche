#pragma once

#include <cstdint>

// VITAL EFFECTS: Shared actor rules, independent of species-specific AI slots.
struct VitalEffects {
    std::uint16_t healing_left = 0;
    std::uint16_t healing_wait = 0;
    std::uint16_t sleep_guard = 0;
    std::uint16_t stun_guard = 0;
    std::uint16_t haste = 0;
    std::uint16_t rooted = 0;
};

struct Entity;
struct Game;
bool apply_sleep(Entity& actor, int ticks);
bool apply_root(Entity& actor, int ticks);
bool apply_stun(Entity& actor, int ticks);
void step_vital_effects(Game& game, int slot);
int movement_recovery_rate(const Entity& actor);
int movement_beat(const Entity& actor, int recovery);
