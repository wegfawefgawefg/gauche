#pragma once

#include <array>
#include <cstddef>

enum class AmbientCue : std::size_t {
    ForestWind, LeafRustle, PineCreak, BranchCreak, BranchFall,
    Stream, WallTrickle, PoolDrips, ReedHiss, Frogs,
    Crickets, TinyBird, DistantCrow, DistantOwl, Woodpecker,
    DeerRustle, BeeHum, DenBreath, RuinCreak, MansionWhisper,
    IceGroan, UnderIce, ThawDrip, ThinWind, WindowWhistle, HangingChain,
    DistantBell, TimberCreak, SnowSettling, BoilerIdle, PressureHiss, PipeKnock,
    Waterwheel, SlushLap, GlassTinkle, ObservatoryMotor, ClothFlutter, FarIceCall,
    ChimneyDraft, SubmergedKnock, BeltRollers, FurnaceBreath, DistantPicks, ChainSway,
    CoolingTicks, SlagBubbling, WaterHammer, ShiftBell, Count,
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
    int maximum_sources = 4;
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
    {"ice_groan", AmbientMode::Occasional, .16F, 1, 17, .020F, 35},
    {"under_ice", AmbientMode::Loop, .10F, 1, 10},
    {"thaw_drip", AmbientMode::Loop, .12F, 1, 8},
    {"thin_wind", AmbientMode::Loop, .055F, 0, 25},
    {"window_whistle", AmbientMode::Loop, .11F, 1, 9},
    {"hanging_chain", AmbientMode::Enter, .18F, 1, 10, 0, 35, 4, true},
    {"distant_bell", AmbientMode::Occasional, .12F, 0, 30, .009F, 70},
    {"timber_creak", AmbientMode::Enter, .15F, 1, 11, 0, 60, 4, true},
    {"snow_settling", AmbientMode::Occasional, .12F, 1, 8, .035F, 25},
    {"boiler_idle", AmbientMode::Loop, .14F, 1, 10},
    {"pressure_hiss", AmbientMode::Loop, .075F, 1, 7},
    {"pipe_knock", AmbientMode::Occasional, .15F, 1, 12, .030F, 24},
    {"waterwheel", AmbientMode::Loop, .14F, 1, 12},
    {"slush_lap", AmbientMode::Loop, .08F, 1, 8},
    {"glass_tinkle", AmbientMode::Enter, .16F, 1, 9, 0, 45, 3, true},
    {"observatory_motor", AmbientMode::Loop, .085F, 1, 9},
    {"cloth_flutter", AmbientMode::Loop, .10F, 1, 8},
    {"far_ice_call", AmbientMode::Occasional, .12F, 0, 30, .015F, 50},
    {"chimney_draft", AmbientMode::Loop, .10F, 1, 9},
    {"submerged_knock", AmbientMode::Occasional, .14F, 1, 12, .020F, 40},
    {"belt_rollers", AmbientMode::Loop, .11F, 1, 12},
    {"furnace_breath", AmbientMode::Loop, .10F, 1, 10, 0, 0, 3, false, 16},
    {"distant_picks", AmbientMode::Occasional, .19F, 2, 24, .08F, 15},
    {"chain_sway", AmbientMode::Loop, .09F, 1, 9, 0, 0, 3, false, 8},
    {"cooling_ticks", AmbientMode::Occasional, .14F, 1, 10, .10F, 12.5F, 3, false, 16},
    {"slag_bubbling", AmbientMode::Loop, .09F, 1, 12, 0, 0, 3, false, 16},
    {"water_hammer", AmbientMode::Occasional, .15F, 1, 14, .04F, 22.5F, 3, false, 8},
    {"shift_bell", AmbientMode::Occasional, .09F, 0, 30, .008F, 75},
}};
