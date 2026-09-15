#pragma once

#include <cstdint>

enum class LightShape : std::uint8_t { Omni, Cone, Beam };

struct LightTint {
    std::uint8_t red = 255, green = 255, blue = 255;
};

struct LightEmitter {
    int radius = 0;
    int strength = 0; // Thousandths of one source unit.
    LightTint color{};
    LightShape shape = LightShape::Omni;
};
