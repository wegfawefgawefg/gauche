#pragma once
#include "actions.hpp"
#include <gubsy/runtime.hpp>
#include <string>

struct InputPrompt {
    std::string label = "--";
    bool gamepad = false;
    SDL_GamepadButtonLabel face = SDL_GAMEPAD_BUTTON_LABEL_UNKNOWN;
};

void set_prompt_runtime(GubsyRuntime& runtime);
InputPrompt action_prompt(Action action, int profile_id = -1);
InputPrompt pad_button_prompt(SDL_GamepadButton button);
InputPrompt stick_prompt(int action, int profile_id);
