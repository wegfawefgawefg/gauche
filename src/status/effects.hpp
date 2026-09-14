#pragma once

#include <cstdint>

enum class RootKind : std::uint8_t { Rope, Net };

// VITAL EFFECTS: Shared actor rules, independent of species-specific AI slots.
struct VitalEffects {
    std::uint16_t healing_left = 0;
    std::uint16_t healing_wait = 0;
    std::uint16_t sleep_guard = 0;
    std::uint16_t stun_guard = 0;
    std::uint16_t haste = 0;
    std::uint16_t rooted = 0;
    std::uint16_t grip = 0;
    RootKind root_kind = RootKind::Rope;
    std::uint16_t nausea = 0, nausea_wait = 0;
};

struct Entity;
struct Game;
bool apply_chill(Entity& actor, int ticks);
bool apply_sleep(Entity& actor, int ticks);
bool apply_nausea(Entity& actor, int ticks);
bool apply_root(Entity& actor, int ticks, RootKind kind = RootKind::Rope);
bool apply_stun(Entity& actor, int ticks);
void step_vital_effects(Game& game, int slot);
int movement_slow_factor(const Entity& actor);
int movement_recovery_rate(const Entity& actor);
int movement_beat(const Entity& actor, int recovery);
