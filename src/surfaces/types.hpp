#pragma once
#include <cstdint>

enum class LiquidKind : std::uint8_t { None, Oil, Sap, Water, Honey, Rot, SpentSap, Brine, Count };
struct Surface {
    LiquidKind liquid = LiquidKind::None;
    std::uint16_t liquid_ticks = 0;
    std::uint16_t fire_ticks = 0;
    std::uint16_t smoke_ticks = 0;
    std::uint16_t sleep_ticks = 0;
    std::uint16_t scent_ticks = 0;
    bool gritted = false;
    std::uint16_t warmth_ticks = 0;
    std::uint16_t whiteout_ticks = 0;
    std::uint16_t still_ticks = 0;
};

inline bool obscures_sight(const Surface& surface) {
    return surface.smoke_ticks >= 60 || surface.whiteout_ticks > 0;
}

inline bool water_liquid(LiquidKind kind) {
    return kind == LiquidKind::Water || kind == LiquidKind::Brine;
}
