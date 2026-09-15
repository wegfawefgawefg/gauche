#pragma once

#include <cstdint>

enum class RootKind : std::uint8_t { Rope, Net };
enum class RecoveryKind : std::uint8_t { Herbs, Broth, Poultice };

// VITAL EFFECTS: Shared actor rules, independent of species-specific AI slots.
struct VitalEffects {
    std::uint16_t healing_left = 0;
    std::uint16_t healing_wait = 0;
    std::uint16_t sleep_guard = 0;
    std::uint16_t stun_guard = 0;
    std::uint16_t haste = 0;
    std::uint16_t rooted = 0;
    std::uint16_t grip = 0;
    std::uint16_t floor_insulation = 0;
    std::uint16_t traction = 0, slide_momentum = 0;
    RootKind root_kind = RootKind::Rope;
    std::uint16_t nausea = 0, nausea_wait = 0;
    std::uint16_t chill_guard = 0;
    std::uint16_t summer_ticks = 0;
    std::uint8_t summer_radius = 0;
    RecoveryKind recovery = RecoveryKind::Herbs;
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
int recovery_interval(const VitalEffects& effects);
bool begin_recovery(Entity& actor, RecoveryKind kind, int health);
void step_recovery(Entity& actor);
bool interrupt_recovery(Entity& actor);
