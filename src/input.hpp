#pragma once

#include "game.hpp"
#include <gubsy/runtime.hpp>

void register_game_bindings(GubsyRuntime& runtime);
struct PointerState {
    float x = 0.0F;
    float y = 0.0F;
    Cell cell{};
    bool inside = false;
    bool left = false;
};
PointerState read_pointer(const GubsyFrame& frame, const Game& game,
                          int owner, float zoom);
Input read_local_input(GubsyRuntime& runtime, const Game& game,
                       const GubsyFrame& frame, int owner, float zoom);
