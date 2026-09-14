#include "pages.hpp"
#include "view_builder.hpp"

#include <gubsy/input/binds_profile.hpp>
#include <gubsy/lobby/state.hpp>

#include <algorithm>
#include <string>

namespace {

using gauche_menu::ViewBuilder;

std::string button_label(int code) {
    ginput::DeviceButton decoded;
    if (!ginput::decode_button(code, decoded))
        return binds_input_label(BindsActionType::Button, code);
    if (decoded.kind == ginput::DeviceKind::Keyboard) {
        const char* name = SDL_GetScancodeName(static_cast<SDL_Scancode>(decoded.code));
        return "Keyboard " + std::string{name == nullptr ? "Unknown" : name};
    }
    if (decoded.kind == ginput::DeviceKind::Gamepad) {
        const char* name = SDL_GetGamepadStringForButton(
            static_cast<SDL_GamepadButton>(decoded.code));
        return "Gamepad " + std::string{name == nullptr ? "Unknown" : name};
    }
    if (decoded.kind == ginput::DeviceKind::Mouse)
        return "Mouse button " + std::to_string(decoded.code);
    return "Device button " + std::to_string(decoded.code);
}

void frame(ViewBuilder& ui, std::string title, float width = 760.0F,
           float height = 520.0F) {
    ui.layout_container("root", "shell", glayout::ContainerKind::Stack,
                        {glayout::LengthKind::Fill, 1.0F},
                        {glayout::LengthKind::Fill, 1.0F});
    ui.container("shell", "card", glayout::ContainerKind::Column,
                 {glayout::LengthKind::Pixels, width},
                 {glayout::LengthKind::Pixels, height}, 8.0F,
                 {24.0F, 20.0F, 24.0F, 20.0F});
    ui.layout("card").align = glayout::Align::Center;
    ui.spec("card").style_class = "large-main-panel";
    ui.label("card", "heading", std::move(title), 70.0F, 36.0F,
             gview::TextAlign::Center);
}

void button(ViewBuilder& ui, std::string id, std::string label,
            std::string action, float height = 46.0F) {
    const std::string node_id = id;
    ui.button("card", std::move(id), std::move(label), std::move(action),
              "menu", height);
    ui.spec(node_id).text_style.size = 19.0F * ui.scale();
}

void footer(ViewBuilder& ui, std::string first) {
    button(ui, "back", "Back", "back", 42.0F);
    ui.focus_group("menu", std::move(first), "card");
}

void main_page(ViewBuilder& ui) {
    frame(ui, "GAUCHE", 520.0F, 385.0F);
    button(ui, "play", "Play", "play", 51.0F);
    button(ui, "quick", "Quick Run", "quick", 51.0F);
    button(ui, "settings", "Settings", "settings", 51.0F);
    button(ui, "quit", "Quit", "quit", 51.0F);
    ui.focus_group("menu", "play", "card");
}

void lobby_page(ViewBuilder& ui, const FrontPage& page, int death_policy) {
    const GubsyLobbyState& lobby = gubsy_get_lobby_state(*page.backend);
    frame(ui, "Custom Game", 840.0F, 558.0F);
    const std::string status = lobby.last_error.empty() ?
        (lobby.online ? (lobby.is_host ? "Hosting · " + lobby.advertised_endpoint :
                        "Joined · " + lobby.status_message) : "Offline lobby") :
        "Error · " + lobby.last_error;
    ui.label("card", "status", status, 32.0F, 16.0F);
    button(ui, "players", "Players  ·  " + std::to_string(lobby.local_players.size()) +
           " local  ·  " + std::to_string(lobby.game_members.size()) + " remote", "players");
    constexpr const char* policies[]{"Next Floor", "Entrance", "No Respawn"};
    button(ui, "rules", std::string{"Game Rules  ·  "} +
           policies[std::clamp(death_policy, 0, 2)], "rules");
    if (!lobby.online || lobby.is_host)
        button(ui, "host", lobby.online ? "Host Details" : "Host Game", "host");
    if (!lobby.online) button(ui, "join", "Join Game", "join");
    if (lobby.online) button(ui, "leave", "Leave Session", "leave");
    button(ui, "start", lobby.online ? "Start Hosted Run" : "Start Local Game", "start");
    footer(ui, "players");
}

void rules_page(ViewBuilder& ui, int policy) {
    frame(ui, "Game Rules", 700.0F, 410.0F);
    ui.label("card", "rule-help", "When can a dead player return?", 38.0F, 17.0F);
    constexpr const char* labels[]{"Next Floor", "Entrance", "No Respawn"};
    constexpr const char* actions[]{"policy:0", "policy:1", "policy:2"};
    for (int index = 0; index < 3; ++index)
        button(ui, "policy-" + std::to_string(index),
               std::string(index == policy ? "●  " : "○  ") + labels[index], actions[index]);
    footer(ui, "policy-0");
}

void host_page(ViewBuilder& ui, const FrontPage& page) {
    const GubsyLobbyState& lobby = gubsy_get_lobby_state(*page.backend);
    frame(ui, "Host Game", 700.0F, 410.0F);
    ui.label("card", "host-help", lobby.online ? "Direct session is open" :
             "Open a direct UDP session for your friends.", 38.0F, 16.0F);
    ui.text_input("card", "host-port", "Port", "host-port", "menu", 48.0F);
    if (!lobby.online) button(ui, "host-open", "Open Direct Lobby", "host:open");
    if (lobby.online && lobby.is_host)
        button(ui, "host-start", "Start Hosted Run", "start");
    footer(ui, "host-port");
}

void join_page(ViewBuilder& ui, const FrontPage& page) {
    const GubsyLobbyState& lobby = gubsy_get_lobby_state(*page.backend);
    frame(ui, "Join Game", 700.0F, 430.0F);
    ui.label("card", "join-help", lobby.direct_join_pending ? "Connecting to host…" :
             "Enter the host's address and UDP port.", 38.0F, 16.0F);
    ui.text_input("card", "join-host", "Host address", "join-host", "menu", 48.0F);
    ui.text_input("card", "join-port", "Port", "join-port", "menu", 48.0F);
    button(ui, "connect", "Connect", "join:connect");
    footer(ui, "join-host");
}

void players_page(ViewBuilder& ui, const FrontPage& page) {
    const GubsyLobbyState& lobby = gubsy_get_lobby_state(*page.backend);
    frame(ui, "Players", 760.0F, 440.0F);
    ui.label("card", "player-count", "One player per machine · four per game", 36.0F, 16.0F);
    for (int index = 0; index < static_cast<int>(lobby.local_players.size()); ++index) {
        const auto& player = lobby.local_players[static_cast<std::size_t>(index)];
        const BindsProfile* profile = gubsy_find_binds_profile(*page.backend,
                                                                player.binds_profile_id);
        button(ui, "player-" + std::to_string(index),
               "Player " + std::to_string(index + 1) + "  ·  " +
               (profile ? profile->name : "Default") + "  ·  Setup", "player:" +
               std::to_string(index));
    }
    footer(ui, "player-0");
}

void player_page(ViewBuilder& ui, const FrontPage& page) {
    const GubsyLobbyState& lobby = gubsy_get_lobby_state(*page.backend);
    frame(ui, "Player Setup", 760.0F, 500.0F);
    if (page.selected_player < static_cast<int>(lobby.local_players.size())) {
        const GubsyLobbyPlayer& player =
            lobby.local_players[static_cast<std::size_t>(page.selected_player)];
        for (const BindsProfile& profile : gubsy_get_binds_profiles(*page.backend))
            button(ui, "player-profile-" + std::to_string(profile.id),
                   std::string(player.binds_profile_id == profile.id ? "●  " : "○  ") +
                   "Controls  ·  " + profile.name, "player:profile:" +
                   std::to_string(profile.id));
        button(ui, "player-keyboard", "Keyboard + Mouse  ·  Toggle",
               "player:keyboard");
        const auto pads = gubsy_get_gamepads(*page.backend);
        for (const GubsyGamepad& pad : pads)
            button(ui, "pad-" + std::to_string(pad.device_id),
                   pad.name + "  ·  Toggle", "player:pad:" +
                   std::to_string(pad.device_id));
    }
    footer(ui, "player-keyboard");
}

void settings_page(ViewBuilder& ui) {
    frame(ui, "Settings", 620.0F, 400.0F);
    button(ui, "display", "Display", "display");
    button(ui, "audio", "Audio", "audio");
    button(ui, "controls", "Controls", "controls");
    footer(ui, "display");
}

void display_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Display Settings", 740.0F, 545.0F);
    constexpr const char* resolutions[]{"640 × 360", "960 × 540", "1280 × 720",
                                        "1920 × 1080"};
    constexpr const char* modes[]{"Windowed", "Borderless", "Fullscreen"};
    constexpr const char* caps[]{"Unlimited", "60 FPS", "120 FPS", "144 FPS"};
    button(ui, "render-resolution", std::string{"Game resolution  ·  "} +
           resolutions[std::clamp(page.render_resolution, 0, 3)], "display:render-resolution");
    button(ui, "window-resolution", std::string{"Window size  ·  "} +
           resolutions[std::clamp(page.window_resolution, 0, 3)], "display:window-resolution");
    button(ui, "window-mode", std::string{"Window mode  ·  "} +
           modes[std::clamp(page.window_mode, 0, 2)], "display:window-mode");
    button(ui, "vsync", std::string{"Vertical sync  ·  "} +
           (page.vsync ? "On" : "Off"), "display:vsync");
    button(ui, "frame-cap", std::string{"Frame cap  ·  "} +
           caps[std::clamp(page.frame_cap, 0, 3)], "display:frame-cap");
    button(ui, "show-fps", std::string{"Show FPS  ·  "} +
           (page.show_fps ? "On" : "Off"), "display:show-fps");
    footer(ui, "render-resolution");
}

void audio_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Audio Settings", 700.0F, 440.0F);
    const auto percent = [](float level) { return std::to_string(static_cast<int>(level * 100)); };
    button(ui, "master", "Master volume  ·  " + percent(page.master_volume) + "%",
           "audio:master");
    button(ui, "music", "Music volume  ·  " + percent(page.music_volume) + "%",
           "audio:music");
    button(ui, "sfx", "Effects volume  ·  " + percent(page.sfx_volume) + "%",
           "audio:sfx");
    ui.label("card", "audio-help", "Select a row to cycle through volume levels.",
             40.0F, 15.0F);
    footer(ui, "master");
}

void controls_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Control Profiles", 790.0F, 500.0F);
    ui.label("card", "control-help", "Edit a profile, then assign it in Player Setup.",
             40.0F, 16.0F);
    for (const BindsProfile& profile : gubsy_get_binds_profiles(*page.backend))
        button(ui, "profile-" + std::to_string(profile.id), profile.name + "  ·  Edit",
               "profile:" + std::to_string(profile.id));
    button(ui, "profile-new", "+ Create Profile", "profile:new");
    footer(ui, gubsy_get_binds_profiles(*page.backend).empty() ? "back" :
               "profile-" + std::to_string(gubsy_get_binds_profiles(*page.backend).front().id));
}

void profile_editor_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Edit · " + page.profile_name, 760.0F, 490.0F);
    ui.text_input("card", "profile-name", "Profile name", "profile-name", "menu", 48.0F);
    button(ui, "profile-save", "Save Name", "profile:save");
    button(ui, "profile-bindings", "Button and Analog Mappings", "profile:bindings");
    button(ui, "profile-input", "Controller Options", "input-options");
    button(ui, "profile-copy", "Duplicate Profile", "profile:copy");
    ui.label("card", "profile-note", "Assign this profile in Player Setup.", 35.0F, 15.0F);
    footer(ui, "profile-name");
}

void bindings_page(ViewBuilder& ui, const FrontPage& page) {
    const BindsProfile* profile = gubsy_find_binds_profile(*page.backend,
                                                            page.selected_profile);
    frame(ui, profile ? profile->name + " · Bindings" : "Bindings", 900.0F, 620.0F);
    ui.layout_container("card", "binding-list", glayout::ContainerKind::Column,
                        {glayout::LengthKind::Fill, 1.0F},
                        {glayout::LengthKind::Fill, 1.0F}, 4.0F);
    ui.scrolling("binding-list");
    const BindsSchema& schema = get_binds_schema();
    for (const ginput::SchemaEntry& entry : schema.actions()) {
        std::string mapping;
        if (profile != nullptr)
            for (const ginput::ButtonBind& bind :
                 ginput::button_binds_for_action(*profile, entry.id)) {
                if (!mapping.empty()) mapping += ", ";
                mapping += button_label(bind.device_button);
            }
        if (mapping.empty()) mapping = "Unbound";
        ui.button("binding-list", "bind-" + std::to_string(entry.id),
                  entry.label + "  ·  " + mapping,
                  "bind:0:" + std::to_string(entry.id),
                  "menu", 39.0F);
    }
    for (const ginput::SchemaEntry& entry : schema.axes_1d())
        ui.button("binding-list", "axis1-" + std::to_string(entry.id),
                  entry.label + "  ·  Analog", "bind:1:" + std::to_string(entry.id),
                  "menu", 39.0F);
    for (const ginput::SchemaEntry& entry : schema.axes_2d())
        ui.button("binding-list", "axis2-" + std::to_string(entry.id),
                  entry.label + "  ·  Analog", "bind:2:" + std::to_string(entry.id),
                  "menu", 39.0F);
    ui.label("card", "binding-help", page.capturing_bind ?
             "Press a key, mouse button, or gamepad button…" :
             "Select an action to view, add, or remove mappings.", 34.0F, 15.0F);
    button(ui, "input-options", "Stick and trigger options", "input-options", 42.0F);
    footer(ui, schema.actions().empty() ? "back" :
               "bind-" + std::to_string(schema.actions().front().id));
}

void input_options_page(ViewBuilder& ui, const FrontPage&) {
    frame(ui, "Controller Options", 760.0F, 520.0F);
    ui.layout_container("card", "input-list", glayout::ContainerKind::Column,
                        {glayout::LengthKind::Fill, 1.0F},
                        {glayout::LengthKind::Fill, 1.0F}, 5.0F);
    ui.scrolling("input-list");
    ui.slider("input-list", "controller-sensitivity", "Controller sensitivity",
              "input:controller-sensitivity", "menu", 0.4, 2.0, 0.05, 46.0F);
    ui.slider("input-list", "stick-deadzone", "Stick deadzone",
              "input:stick-deadzone", "menu", 0.0, 0.95, 0.01, 46.0F);
    ui.slider("input-list", "trigger-threshold", "Trigger threshold",
              "input:trigger-threshold", "menu", 0.01, 1.0, 0.01, 46.0F);
    ui.toggle("input-list", "controller-invert-x", "Invert controller X",
              "input:controller-invert-x", "menu", 42.0F);
    ui.toggle("input-list", "controller-invert-y", "Invert controller Y",
              "input:controller-invert-y", "menu", 42.0F);
    ui.label("card", "mouse-note", "Mouse aim follows the pointer directly.",
             32.0F, 16.0F);
    footer(ui, "controller-sensitivity");
}

void binding_detail_page(ViewBuilder& ui, const FrontPage& page) {
    const BindsProfile* profile = gubsy_find_binds_profile(*page.backend,
                                                            page.selected_profile);
    const BindsSchema& schema = get_binds_schema();
    const ginput::SchemaEntry* entry = page.selected_bind_type == BindsActionType::Button ?
        schema.find_action(page.selected_bind_action) :
        (page.selected_bind_type == BindsActionType::Analog1D ?
         schema.find_axis_1d(page.selected_bind_action) :
         schema.find_axis_2d(page.selected_bind_action));
    frame(ui, entry ? entry->label : "Binding", 760.0F, 520.0F);
    ui.label("card", "binding-instruction", "Existing mappings", 36.0F, 17.0F);
    int index = 0;
    if (profile != nullptr && page.selected_bind_type == BindsActionType::Button)
        for (const ginput::ButtonBind& mapping :
             ginput::button_binds_for_action(*profile, page.selected_bind_action)) {
            button(ui, "mapping-" + std::to_string(index),
                   button_label(mapping.device_button) +
                   "  ·  Remove", "bind:remove:" + std::to_string(index));
            ++index;
        }
    if (profile != nullptr && page.selected_bind_type == BindsActionType::Analog1D)
        for (const ginput::Axis1DBind& mapping :
             ginput::binds_for_axis_1d(*profile, page.selected_bind_action)) {
            button(ui, "mapping-" + std::to_string(index),
                   binds_input_label(BindsActionType::Analog1D, mapping.device_axis) +
                   "  ·  Remove", "bind:remove:" + std::to_string(index));
            ++index;
        }
    if (profile != nullptr && page.selected_bind_type == BindsActionType::Analog2D)
        for (const ginput::Axis2DBind& mapping :
             ginput::binds_for_axis_2d(*profile, page.selected_bind_action)) {
            button(ui, "mapping-" + std::to_string(index),
                   binds_input_label(BindsActionType::Analog2D, mapping.device_stick) +
                   "  ·  Remove", "bind:remove:" + std::to_string(index));
            ++index;
        }
    if (index == 0) ui.label("card", "no-mapping", "No mappings yet", 40.0F, 15.0F);
    button(ui, "binding-add", page.capturing_bind ? "Press a button…" :
           "+ Add Mapping", "bind:add");
    button(ui, "binding-clear", "Clear Action", "bind:clear");
    footer(ui, "binding-add");
}

void binding_choices_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Choose Analog Input", 720.0F, 510.0F);
    ui.layout_container("card", "choice-list", glayout::ContainerKind::Column,
                        {glayout::LengthKind::Fill, 1.0F},
                        {glayout::LengthKind::Fill, 1.0F}, 4.0F);
    ui.scrolling("choice-list");
    const std::vector<InputChoice>& choices = binds_input_choices(page.selected_bind_type);
    for (std::size_t index = 0; index < choices.size(); ++index)
        ui.button("choice-list", "choice-" + std::to_string(index), choices[index].label,
                  "bind:choice:" + std::to_string(index), "menu", 42.0F);
    footer(ui, choices.empty() ? "back" : "choice-0");
}

void pause_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Paused", 560.0F, 410.0F);
    button(ui, "resume", "Resume", "resume");
    if (page.allow_restart) button(ui, "restart", "Restart Run", "restart");
    button(ui, "pause-settings", "Settings", "settings");
    button(ui, "title", "Quit to Main Menu", "title");
    ui.focus_group("menu", "resume", "card");
}

} // namespace

gview::View build_menu_page(const FrontPage& page, int width, int height,
                            int death_policy) {
    ViewBuilder ui("gauche-menu", width, height);
    switch (page.screen) {
    case MenuScreen::Main: main_page(ui); break;
    case MenuScreen::Lobby: lobby_page(ui, page, death_policy); break;
    case MenuScreen::Rules: rules_page(ui, death_policy); break;
    case MenuScreen::Host: host_page(ui, page); break;
    case MenuScreen::Join: join_page(ui, page); break;
    case MenuScreen::Players: players_page(ui, page); break;
    case MenuScreen::Player: player_page(ui, page); break;
    case MenuScreen::Settings: settings_page(ui); break;
    case MenuScreen::Display: display_page(ui, page); break;
    case MenuScreen::Audio: audio_page(ui, page); break;
    case MenuScreen::Controls: controls_page(ui, page); break;
    case MenuScreen::ProfileEditor: profile_editor_page(ui, page); break;
    case MenuScreen::Bindings: bindings_page(ui, page); break;
    case MenuScreen::BindDetail: binding_detail_page(ui, page); break;
    case MenuScreen::BindChoices: binding_choices_page(ui, page); break;
    case MenuScreen::InputOptions: input_options_page(ui, page); break;
    case MenuScreen::Pause: pause_page(ui, page); break;
    }
    if (!page.toast.empty()) {
        ui.label("card", "toast", page.toast, 28.0F, 14.0F,
                 gview::TextAlign::Center);
    }
    return ui.finish();
}
