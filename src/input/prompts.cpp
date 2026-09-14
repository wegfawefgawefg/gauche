#include "prompts.hpp"
#include "../input.hpp"
#include <gubsy/lobby/state.hpp>
#include <gubsy/input/binds_profile.hpp>

namespace {

GubsyRuntime* backend = nullptr;

int controller_family() {
    SDL_Gamepad* pad = SDL_GetGamepadFromID(active_gamepad_id());
    const SDL_GamepadType type = pad == nullptr ? SDL_GAMEPAD_TYPE_UNKNOWN : SDL_GetGamepadType(pad);
    if (type == SDL_GAMEPAD_TYPE_PS3 || type == SDL_GAMEPAD_TYPE_PS4 || type == SDL_GAMEPAD_TYPE_PS5) return 1;
    if (type >= SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO && type <= SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR) return 2;
    return 0;
}

InputPrompt gamepad_button(int code) {
    InputPrompt prompt;
    prompt.gamepad = true;
    SDL_Gamepad* gamepad = SDL_GetGamepadFromID(active_gamepad_id());
    const auto button = static_cast<SDL_GamepadButton>(code);
    prompt.face = gamepad == nullptr ? SDL_GAMEPAD_BUTTON_LABEL_UNKNOWN : SDL_GetGamepadButtonLabel(gamepad, button);
    switch (prompt.face) {
    case SDL_GAMEPAD_BUTTON_LABEL_A: prompt.label = "A"; break;
    case SDL_GAMEPAD_BUTTON_LABEL_B: prompt.label = "B"; break;
    case SDL_GAMEPAD_BUTTON_LABEL_X: prompt.label = "X"; break;
    case SDL_GAMEPAD_BUTTON_LABEL_Y: prompt.label = "Y"; break;
    case SDL_GAMEPAD_BUTTON_LABEL_CROSS: prompt.label = "Cross"; break;
    case SDL_GAMEPAD_BUTTON_LABEL_CIRCLE: prompt.label = "Circle"; break;
    case SDL_GAMEPAD_BUTTON_LABEL_SQUARE: prompt.label = "Square"; break;
    case SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE: prompt.label = "Triangle"; break;
    default:
        const int family = controller_family();
        switch (button) {
        case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER: prompt.label = family == 1 ? "L1" : family == 2 ? "L" : "LB"; break;
        case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER: prompt.label = family == 1 ? "R1" : family == 2 ? "R" : "RB"; break;
        case SDL_GAMEPAD_BUTTON_LEFT_STICK: prompt.label = "L3"; break;
        case SDL_GAMEPAD_BUTTON_RIGHT_STICK: prompt.label = "R3"; break;
        case SDL_GAMEPAD_BUTTON_BACK: prompt.label = family == 2 ? "-" : family == 1 ? "Share" : "View"; break;
        case SDL_GAMEPAD_BUTTON_START: prompt.label = family == 2 ? "+" : family == 1 ? "Options" : "Menu"; break;
        default: {
            const char* label = SDL_GetGamepadStringForButton(button);
            prompt.label = label == nullptr ? "Pad" : label;
            break;
        }
        }
    }
    return prompt;
}

} // namespace

void set_prompt_runtime(GubsyRuntime& runtime) { backend = &runtime; }

InputPrompt action_prompt(Action action, int profile_id) {
    const bool pad = controller_input_active();
    const BindsProfile* profile = nullptr;
    if (backend != nullptr) {
        const auto& players = gubsy_get_lobby_state(*backend).local_players;
        if (profile_id < 0 && !players.empty()) profile_id = players.front().binds_profile_id;
        profile = gubsy_find_binds_profile(*backend, profile_id);
    }
    static const BindsProfile fallback = default_game_binds();
    if (profile == nullptr) profile = &fallback;
    for (const auto& binding : ginput::button_binds_for_action(*profile, action_id(action))) {
        ginput::DeviceButton button;
        if (!ginput::decode_button(binding.device_button, button)) {
            // LEGACY: Gubsy's built-in profiles predate encoded device controls.
            const int code = binding.device_button;
            if (pad && code >= static_cast<int>(GubsyButton::GP_A) && code < static_cast<int>(GubsyButton::COUNT))
                return gamepad_button(code - static_cast<int>(GubsyButton::GP_A));
            if (!pad && code >= 0 && code < static_cast<int>(GubsyButton::GP_A)) {
                std::string label = binds_input_label(BindsActionType::Button, code);
                if (label.starts_with("Keyboard ")) label.erase(0, 9);
                return {label, false};
            }
            continue;
        }
        if (pad && button.kind == ginput::DeviceKind::Gamepad) return gamepad_button(button.code);
        if (!pad && button.kind == ginput::DeviceKind::Keyboard)
            return {SDL_GetScancodeName(static_cast<SDL_Scancode>(button.code)), false};
        if (!pad && button.kind == ginput::DeviceKind::Mouse)
            return {"M" + std::to_string(button.code), false};
    }
    if (pad && action == Action::Use)
        for (const auto& binding : ginput::binds_for_axis_1d(*profile, 0)) {
            ginput::DeviceAxis1D axis;
            bool decoded = ginput::decode_axis_1d(binding.device_axis, axis);
            if (!decoded && (binding.device_axis == static_cast<int>(Gubsy1DAnalog::GP_LEFT_TRIGGER) ||
                binding.device_axis == static_cast<int>(Gubsy1DAnalog::GP_RIGHT_TRIGGER))) {
                axis.kind = ginput::DeviceKind::Gamepad;
                axis.code = binding.device_axis == static_cast<int>(Gubsy1DAnalog::GP_LEFT_TRIGGER) ?
                    SDL_GAMEPAD_AXIS_LEFT_TRIGGER : SDL_GAMEPAD_AXIS_RIGHT_TRIGGER;
                decoded = true;
            }
            if (decoded && axis.kind == ginput::DeviceKind::Gamepad) {
                const int family = controller_family();
                if (axis.code == SDL_GAMEPAD_AXIS_LEFT_TRIGGER)
                    return {family == 1 ? "L2" : family == 2 ? "ZL" : "LT", true};
                if (axis.code == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)
                    return {family == 1 ? "R2" : family == 2 ? "ZR" : "RT", true};
                return {"Axis", true};
            }
        }
    return {"--", pad};
}
