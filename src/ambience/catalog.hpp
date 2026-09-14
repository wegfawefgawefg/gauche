#pragma once

#include <array>
#include <cstddef>

enum class AmbientCue : std::size_t {
    ForestWind, LeafRustle, PineCreak, BranchCreak, BranchFall,
    Stream, WallTrickle, PoolDrips, ReedHiss, Frogs,
    Crickets, TinyBird, DistantCrow, DistantOwl, Woodpecker,
    DeerRustle, BeeHum, DenBreath, RuinCreak, MansionWhisper, Count,
};
enum class AmbientMode { Loop, Enter, Occasional };
struct AmbientSpec {
    const char* name;
    AmbientMode mode;
    float gain, near_radius, far_radius;
    float chance_per_second = 0;
    float cooldown = 0;
    float trigger_radius = 3;
    bool rearm = false;
};

inline constexpr std::array<AmbientSpec, static_cast<std::size_t>(AmbientCue::Count)> ambient_specs{{
    {"forest_wind", AmbientMode::Loop, .10F, 0, 25},
    {"leaf_rustle", AmbientMode::Loop, .16F, 2, 10},
    {"pine_creak", AmbientMode::Occasional, .21F, 1, 12, .035F, 24},
    {"branch_creak", AmbientMode::Enter, .26F, 1, 9, 0, 30, 3, true},
    {"branch_fall", AmbientMode::Occasional, .17F, 0, 28, .012F, 45},
    {"stream", AmbientMode::Loop, .25F, 2, 14},
    {"wall_trickle", AmbientMode::Loop, .20F, 1, 9},
    {"pool_drips", AmbientMode::Occasional, .18F, 1, 8, .20F, 4},
    {"reed_hiss", AmbientMode::Loop, .08F, 1, 7},
    {"frogs", AmbientMode::Occasional, .22F, 2, 14, .08F, 12},
    {"crickets", AmbientMode::Loop, .065F, 0, 18},
    {"tiny_bird", AmbientMode::Occasional, .22F, 2, 22, .08F, 10},
    {"distant_crow", AmbientMode::Occasional, .20F, 0, 28, .025F, 25},
    {"distant_owl", AmbientMode::Occasional, .15F, 0, 30, .013F, 35},
    {"woodpecker", AmbientMode::Occasional, .17F, 1, 17, .035F, 18},
    {"deer_rustle", AmbientMode::Enter, .23F, 1, 9, 0, 30, 4, true},
    {"bee_hum", AmbientMode::Loop, .13F, 1, 8},
    {"den_breath", AmbientMode::Loop, .13F, 1, 8},
    {"ruin_creak", AmbientMode::Enter, .22F, 1, 13, 0, 0, 4},
    {"mansion_whisper", AmbientMode::Enter, .18F, 1, 14, 0, 0, 7},
}};
