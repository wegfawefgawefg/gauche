#pragma once

#include "system.hpp"

void spawn_footprint(Cosmetics& cosmetics, Cell cell, EntityKind kind,
                     bool right_foot, std::uint64_t seed);
void spawn_hit(Cosmetics& cosmetics, Cell cell, std::uint64_t seed, int damage);
void spawn_zombie_scratch(Cosmetics& cosmetics, Cell cell, Cell facing,
                          std::uint64_t seed);
void spawn_debris(Cosmetics& cosmetics, Cell cell, std::uint64_t seed);
void spawn_death(Cosmetics& cosmetics, Cell cell, EntityKind kind,
                 float angle, std::uint64_t seed);
void spawn_sound_effect(Cosmetics& cosmetics, const SoundEvent& sound,
                        std::uint64_t seed);
void spawn_weather_cloud(Cosmetics& cosmetics, Cell focus, std::uint64_t seed,
                         float zoom = 2.0F);
void spawn_campfire_smoke(Cosmetics& cosmetics, Cell cell, std::uint64_t seed);
void spawn_flame(Cosmetics& cosmetics, Cell cell, std::uint64_t seed, bool on_actor);
void spawn_terrain_impact(Cosmetics& cosmetics, const ImpactEvent& impact,
                           std::uint64_t seed);
