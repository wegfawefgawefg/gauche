#pragma once

#include "game.hpp"
#include "view.hpp"
#include <gubsy/runtime.hpp>

void register_game_bindings(GubsyRuntime& runtime);
void observe_input_device(const SDL_Event& event);
bool pointer_device_active();
bool inventory_button_down(GubsyRuntime& runtime);
bool compare_button_down(GubsyRuntime& runtime);
bool compact_details_button_down(GubsyRuntime& runtime);
struct PointerState {
    float x = 0.0F;
    float y = 0.0F;
    Cell cell{};
    bool inside = false;
    bool left = false;
};
struct InputReaderState {
    bool previous_slot_down = false;
    bool next_slot_down = false;
};
PointerState read_pointer(const GubsyFrame& frame, const Game& game,
                          int owner, float zoom, ViewCamera camera);
Input read_local_input(GubsyRuntime& runtime, const Game& game,
                       const GubsyFrame& frame, int owner, float zoom, ViewCamera camera,
                       InputReaderState& reader);
