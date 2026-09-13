#pragma once

#include "game.hpp"
#include <gubsy/runtime.hpp>

void register_game_bindings(GubsyRuntime& runtime);
Input read_local_input(GubsyRuntime& runtime, const Game& game,
                       const GubsyFrame& frame, int owner, float zoom);
