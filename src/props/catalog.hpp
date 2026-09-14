#pragma once

#include "../graphics.hpp"
#include "../sound.hpp"

#include <cstdint>

enum class PropKind : std::uint8_t { None, Leaves, Twigs, Fern, TallGrass,
    Puffball, RottenLog, Crate, Nest, ClayPot, Count };

// STORAGE: One four-byte prop per tile, independent of actor slots and inventories.
struct Prop {
    PropKind kind = PropKind::None;
    std::uint8_t hp = 0;
    std::uint8_t variant = 0;
    bool broken = false;
};
static_assert(sizeof(Prop) == 4);

struct PropSpec {
    Sprite sprite;
    SoundId sound;
    int health;
    bool blocking;
    bool breaks_on_step;
};

PropSpec prop_spec(PropKind kind);
bool prop_blocks(const Prop& prop);
