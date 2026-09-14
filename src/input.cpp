#include "input.hpp"
#include "view.hpp"

#include <gubsy/input/types.hpp>
#include <gubsy/input/profile_settings.hpp>
#include <gubsy/lobby/state.hpp>
#include "src/engine_state.hpp"

#include <algorithm>
#include <cmath>

namespace {

enum class Action : int {
    MoveUp, MoveDown, MoveLeft, MoveRight,
    AimUp, AimDown, AimLeft, AimRight,
    Use, Pickup, Drop, Reload, Interact, Confirm,
    Slot1, Slot2, Slot3, Slot4, Slot5, Slot6,
    PreviousSlot, NextSlot,
};

constexpr int action_id(Action action) { return static_cast<int>(action); }

bool down(GubsyRuntime& runtime, Action action) {
    return gubsy_lobby_player_action_down(runtime, 0, action_id(action));
}

void bind(BindsProfile& profile, GubsyButton button, Action action) {
    (void)ginput::add_button_bind(profile,
        ginput::ButtonBind{static_cast<int>(button), action_id(action)});
}

void default_binds(BindsProfile& profile) {
    bind(profile, GubsyButton::KB_W, Action::MoveUp);
    bind(profile, GubsyButton::KB_S, Action::MoveDown);
    bind(profile, GubsyButton::KB_A, Action::MoveLeft);
    bind(profile, GubsyButton::KB_D, Action::MoveRight);
    bind(profile, GubsyButton::KB_UP, Action::AimUp);
    bind(profile, GubsyButton::KB_DOWN, Action::AimDown);
    bind(profile, GubsyButton::KB_LEFT, Action::AimLeft);
    bind(profile, GubsyButton::KB_RIGHT, Action::AimRight);
    bind(profile, GubsyButton::KB_SPACE, Action::Use);
    bind(profile, GubsyButton::KB_E, Action::Pickup);
    bind(profile, GubsyButton::KB_Q, Action::Drop);
    bind(profile, GubsyButton::KB_R, Action::Reload);
    bind(profile, GubsyButton::KB_F, Action::Interact);
    bind(profile, GubsyButton::KB_ENTER, Action::Confirm);

    bind(profile, GubsyButton::GP_DPAD_UP, Action::MoveUp);
    bind(profile, GubsyButton::GP_DPAD_DOWN, Action::MoveDown);
    bind(profile, GubsyButton::GP_DPAD_LEFT, Action::MoveLeft);
    bind(profile, GubsyButton::GP_DPAD_RIGHT, Action::MoveRight);
    bind(profile, GubsyButton::GP_X, Action::Pickup);
    bind(profile, GubsyButton::GP_Y, Action::Drop);
    bind(profile, GubsyButton::GP_B, Action::Reload);
    bind(profile, GubsyButton::GP_A, Action::Interact);
    bind(profile, GubsyButton::GP_A, Action::Confirm);
    bind(profile, GubsyButton::GP_LEFT_SHOULDER, Action::PreviousSlot);
    bind(profile, GubsyButton::GP_RIGHT_SHOULDER, Action::NextSlot);
    (void)ginput::add_axis_2d_bind(profile,
        ginput::Axis2DBind{static_cast<int>(Gubsy2DAnalog::GP_LEFT_STICK), 0});
    (void)ginput::add_axis_2d_bind(profile,
        ginput::Axis2DBind{static_cast<int>(Gubsy2DAnalog::GP_RIGHT_STICK), 1});
    (void)ginput::add_axis_1d_bind(profile,
        ginput::Axis1DBind{static_cast<int>(Gubsy1DAnalog::GP_RIGHT_TRIGGER), 0,
                           1.0F, 0.1F});

    constexpr GubsyButton numbers[]{GubsyButton::KB_1, GubsyButton::KB_2,
        GubsyButton::KB_3, GubsyButton::KB_4, GubsyButton::KB_5, GubsyButton::KB_6};
    constexpr GubsyButton keypad[]{GubsyButton::KB_KP_1, GubsyButton::KB_KP_2,
        GubsyButton::KB_KP_3, GubsyButton::KB_KP_4, GubsyButton::KB_KP_5,
        GubsyButton::KB_KP_6};
    for (int index = 0; index < quick_slots; ++index)
        bind(profile, numbers[index], static_cast<Action>(action_id(Action::Slot1) + index));
    for (int index = 0; index < quick_slots; ++index)
        bind(profile, keypad[index], static_cast<Action>(action_id(Action::Slot1) + index));
}

int direction(bool negative, bool positive) {
    return negative ? -1 : (positive ? 1 : 0);
}

// What if a player changes controller tuning? Apply it before reducing analog
// movement to the game's eight tile directions.
ginput::Vec2 tuned_stick(ginput::Vec2 stick, const InputSettingsProfile* tuning) {
    if (tuning == nullptr) return stick;
    const float threshold = std::max(0.05F, tuning->stick_deadzone);
    stick.x = std::abs(stick.x) < threshold ? 0.0F : stick.x * tuning->controller_sensitivity;
    stick.y = std::abs(stick.y) < threshold ? 0.0F : stick.y * tuning->controller_sensitivity;
    if (tuning->controller_invert_x) stick.x = -stick.x;
    if (tuning->controller_invert_y) stick.y = -stick.y;
    return stick;
}

bool has_bind(const BindsProfile& profile, GubsyButton button, Action action) {
    for (const ginput::ButtonBind& binding :
         ginput::button_binds_for_action(profile, action_id(action)))
        if (binding.device_button == static_cast<int>(button)) return true;
    return false;
}

void migrate_old_controller_defaults(BindsProfile& profile) {
    constexpr GubsyButton dpad[]{GubsyButton::GP_DPAD_UP, GubsyButton::GP_DPAD_DOWN,
        GubsyButton::GP_DPAD_LEFT, GubsyButton::GP_DPAD_RIGHT};
    constexpr Action aim[]{Action::AimUp, Action::AimDown, Action::AimLeft, Action::AimRight};
    constexpr Action move[]{Action::MoveUp, Action::MoveDown,
        Action::MoveLeft, Action::MoveRight};
    for (int index = 0; index < 4; ++index) {
        (void)ginput::remove_button_bind(profile,
            {static_cast<int>(dpad[index]), action_id(aim[index])});
        bind(profile, dpad[index], move[index]);
    }
    (void)ginput::remove_button_bind(profile,
        {static_cast<int>(GubsyButton::GP_RIGHT_SHOULDER), action_id(Action::Use)});
    bind(profile, GubsyButton::GP_LEFT_SHOULDER, Action::PreviousSlot);
    bind(profile, GubsyButton::GP_RIGHT_SHOULDER, Action::NextSlot);
    (void)ginput::add_axis_2d_bind(profile,
        {static_cast<int>(Gubsy2DAnalog::GP_RIGHT_STICK), 1});
    (void)ginput::add_axis_1d_bind(profile,
        {static_cast<int>(Gubsy1DAnalog::GP_RIGHT_TRIGGER), 0, 1.0F, 0.1F});
}

} // namespace

void register_game_bindings(GubsyRuntime& runtime) {
    BindsSchema schema;
    schema.add_action(action_id(Action::MoveUp), "Move Up", "Movement");
    schema.add_action(action_id(Action::MoveDown), "Move Down", "Movement");
    schema.add_action(action_id(Action::MoveLeft), "Move Left", "Movement");
    schema.add_action(action_id(Action::MoveRight), "Move Right", "Movement");
    schema.add_action(action_id(Action::AimUp), "Aim / Use Up", "Combat");
    schema.add_action(action_id(Action::AimDown), "Aim / Use Down", "Combat");
    schema.add_action(action_id(Action::AimLeft), "Aim / Use Left", "Combat");
    schema.add_action(action_id(Action::AimRight), "Aim / Use Right", "Combat");
    schema.add_action(action_id(Action::Use), "Use Held Item", "Combat");
    schema.add_action(action_id(Action::Pickup), "Pick Up", "Items");
    schema.add_action(action_id(Action::Drop), "Drop", "Items");
    schema.add_action(action_id(Action::Reload), "Reload", "Combat");
    schema.add_action(action_id(Action::Interact), "Interact", "World");
    schema.add_action(action_id(Action::Confirm), "Confirm Choice", "World");
    for (int index = 0; index < quick_slots; ++index)
        schema.add_action(action_id(Action::Slot1) + index,
                          "Slot " + std::to_string(index + 1), "Items");
    schema.add_action(action_id(Action::PreviousSlot), "Previous Slot", "Items");
    schema.add_action(action_id(Action::NextSlot), "Next Slot", "Items");
    schema.add_axis_2d(0, "Analog Move", "Movement");
    schema.add_axis_2d(1, "Analog Aim", "Combat");
    schema.add_axis_1d(0, "Use Trigger", "Combat");
    gubsy_register_binds_schema(runtime, schema);

    const BindsProfile* existing = nullptr;
    for (const BindsProfile& profile : gubsy_get_binds_profiles(runtime))
        if (profile.name == "DefaultBinds" || profile.name == "Default") existing = &profile;
    if (existing != nullptr &&
        !ginput::button_binds_for_action(*existing, action_id(Action::MoveUp)).empty()) {
        if (has_bind(*existing, GubsyButton::GP_DPAD_UP, Action::AimUp) &&
            has_bind(*existing, GubsyButton::GP_RIGHT_SHOULDER, Action::Use)) {
            BindsProfile migrated = *existing;
            migrate_old_controller_defaults(migrated);
            (void)gubsy_replace_binds_profile(runtime, migrated);
        }
        return;
    }
    BindsProfile profile;
    profile.id = existing == nullptr ? 1 : existing->id;
    profile.name = existing == nullptr ? "DefaultBinds" : existing->name;
    default_binds(profile);
    (void)gubsy_replace_binds_profile(runtime, profile);
}

Input read_local_input(GubsyRuntime& runtime, const Game& game,
                       const GubsyFrame& frame, int owner, float zoom,
                       InputReaderState& reader) {
    Input input;
    const InputSettingsProfile* tuning = gubsy_lobby_input_settings_profile(
        gubsy_runtime_engine(runtime), 0);
    input.move.x = direction(down(runtime, Action::MoveLeft),
                             down(runtime, Action::MoveRight));
    input.move.y = direction(down(runtime, Action::MoveUp),
                             down(runtime, Action::MoveDown));
    if (input.move == Cell{}) {
        const ginput::Vec2 stick = tuned_stick(
            gubsy_lobby_player_axis_2d(runtime, 0, 0), tuning);
        input.move.x = stick.x < -0.35F ? -1 : (stick.x > 0.35F ? 1 : 0);
        input.move.y = stick.y < -0.35F ? -1 : (stick.y > 0.35F ? 1 : 0);
    }
    input.aim.x = direction(down(runtime, Action::AimLeft),
                            down(runtime, Action::AimRight));
    input.aim.y = direction(down(runtime, Action::AimUp),
                            down(runtime, Action::AimDown));
    input.use = input.aim != Cell{} || down(runtime, Action::Use) ||
                gubsy_lobby_player_axis_1d_down(runtime, 0, 0,
                    tuning == nullptr ? 0.35F :
                    std::max(0.01F, tuning->trigger_threshold));
    const ginput::Vec2 aim_stick = tuned_stick(
        gubsy_lobby_player_axis_2d(runtime, 0, 1), tuning);
    const Cell analog_aim{aim_stick.x < -0.35F ? -1 : (aim_stick.x > 0.35F ? 1 : 0),
                          aim_stick.y < -0.35F ? -1 : (aim_stick.y > 0.35F ? 1 : 0)};
    if (analog_aim != Cell{}) input.aim = analog_aim;
    input.pickup = down(runtime, Action::Pickup);
    input.drop = down(runtime, Action::Drop);
    input.reload = down(runtime, Action::Reload);
    input.interact = down(runtime, Action::Interact) && !input.pickup;
    input.confirm = down(runtime, Action::Confirm);
    for (int index = 0; index < quick_slots; ++index)
        if (gubsy_lobby_player_action_down(runtime, 0, action_id(Action::Slot1) + index))
            input.select = index;
    const bool previous = down(runtime, Action::PreviousSlot);
    const bool next = down(runtime, Action::NextSlot);
    if (input.select < 0 && owner >= 0 && owner < 4) {
        const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
        if (player != nullptr) {
            if (next && !reader.next_slot_down)
                input.select = (player->inventory.selected + 1) % quick_slots;
            else if (previous && !reader.previous_slot_down)
                input.select = (player->inventory.selected + quick_slots - 1) % quick_slots;
        }
    }
    reader.previous_slot_down = previous;
    reader.next_slot_down = next;

    const PointerState pointer = read_pointer(frame, game, owner, zoom);
    if (pointer.inside && pointer.left) {
        const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
        input.aim = pointer.cell - player->cell;
        input.use = true;
    }
    return input;
}

PointerState read_pointer(const GubsyFrame& frame, const Game& game,
                          int owner, float zoom) {
    PointerState pointer;
    if (frame.window == nullptr || owner < 0 || owner >= 4) return pointer;
    float mouse_x = 0.0F;
    float mouse_y = 0.0F;
    const SDL_MouseButtonFlags buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    pointer.left = (buttons & SDL_BUTTON_LMASK) != 0;
    int window_w = 0;
    int window_h = 0;
    SDL_GetWindowSize(frame.window, &window_w, &window_h);
    if (window_w <= 0 || window_h <= 0 || frame.render_width <= 0 || frame.render_height <= 0)
        return pointer;
    const float scale = std::min(static_cast<float>(window_w) /
                                     static_cast<float>(frame.render_width),
                                 static_cast<float>(window_h) /
                                     static_cast<float>(frame.render_height));
    const float left = (static_cast<float>(window_w) -
                        static_cast<float>(frame.render_width) * scale) * 0.5F;
    const float top = (static_cast<float>(window_h) -
                       static_cast<float>(frame.render_height) * scale) * 0.5F;
    const float render_x = (mouse_x - left) / scale;
    const float render_y = (mouse_y - top) / scale;
    const Entity* player = owner >= 0 && owner < 4 ?
        get_entity(game, game.players[static_cast<std::size_t>(owner)]) : nullptr;
    if (player == nullptr || render_x < 0.0F || render_y < 0.0F ||
        render_x >= static_cast<float>(frame.render_width) ||
        render_y >= static_cast<float>(frame.render_height)) return pointer;
    pointer.x = render_x * view_width / static_cast<float>(frame.render_width);
    pointer.y = render_y * view_height / static_cast<float>(frame.render_height);
    pointer.cell.x = player->cell.x + static_cast<int>(std::floor(
        (pointer.x - view_center_x) / tile_pixels(zoom)));
    pointer.cell.y = player->cell.y + static_cast<int>(std::floor(
        (pointer.y - view_center_y) / tile_pixels(zoom)));
    pointer.inside = true;
    return pointer;
}
