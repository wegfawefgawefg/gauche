#pragma once
#include <cstdint>

enum class LiquidKind : std::uint8_t { None, Oil, Sap, Water, Honey, Rot, SpentSap, Count };
struct Surface {
    LiquidKind liquid = LiquidKind::None;
    std::uint16_t liquid_ticks = 0;
    std::uint16_t fire_ticks = 0;
    std::uint16_t smoke_ticks = 0;
    std::uint16_t sleep_ticks = 0;
    std::uint16_t scent_ticks = 0;
    bool gritted = false;
};
