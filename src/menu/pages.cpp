#include "../debug/levels.hpp"
#include "../input/icon_set.hpp"
#include "pages.hpp"
#include "page_chrome.hpp"
#include "control_pages.hpp"
#include "control_reference.hpp"
#include "profiles.hpp"
#include "rooms.hpp"

#include <gubsy/input/binds_profile.hpp>
#include <gubsy/lobby/state.hpp>

#include <algorithm>
#include <string>

namespace {

using namespace gauche_menu;

void main_page(ViewBuilder& ui) {
    frame(ui, "TEEMING", 560.0F, 490.0F);
    button(ui, "quick", "Quick Run", "quick", 51.0F);
    button(ui, "play", "Play", "play", 51.0F);
    button(ui, "settings", "Settings", "settings", 51.0F);
    if (GAUCHE_DEV_MODE) button(ui, "dev", "Dev", "dev", 42.0F);
#ifndef __EMSCRIPTEN__
    button(ui, "quit", "Quit", "quit", 51.0F);
#endif
    if (!playtest_summary().empty()) ui.label("card", "test-overrides", playtest_summary(), 52, 14);
    ui.focus_group("menu", "quick", "card");
}

void lobby_page(ViewBuilder& ui, const FrontPage& page, int death_policy) {
    const GubsyLobbyState& lobby = gubsy_get_lobby_state(*page.backend);
    frame(ui, "Play", 710, 550);
    if (page.room_active) {
        button(ui, "party", "Return to Party", "party", 52);
        button(ui, "leave", "Leave Room", "leave");
    } else if (lobby.online) {
        button(ui, "start", "Start Direct Game", "start", 52);
        button(ui, "leave", "Leave Direct Session", "leave");
    } else {
        button(ui, "host", "Host Game", "host", 52);
        button(ui, "join", "Join Game", "join", 52);
        button(ui, "start", "Play Locally", "start", 46);
    }
    constexpr const char* policies[]{"Next Floor", "Entrance", "No Respawn"};
    button(ui, "rules", std::string{"Game Rules  ·  "} +
           policies[std::clamp(death_policy, 0, 2)], "rules", 40);
    button(ui, "players", "Player Setup", "players", 40);
    button(ui, "network-options", "Advanced Connection Options", "network-options", 34);
    footer(ui, page.room_active ? "party" : (lobby.online ? "start" : "host"));
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

void direct_host_page(ViewBuilder& ui, const FrontPage& page) {
    const GubsyLobbyState& lobby = gubsy_get_lobby_state(*page.backend);
    frame(ui, "Direct Host (LAN / IP)", 700.0F, 410.0F);
    ui.label("card", "host-help", lobby.online ? "Direct session is open" :
             "Open a direct UDP session for your friends.", 38.0F, 16.0F);
    ui.text_input("card", "host-port", "Port", "host-port", "menu", 48.0F);
    if (!lobby.online) button(ui, "host-open", "Open Direct Lobby", "host:open");
    if (lobby.online && lobby.is_host)
        button(ui, "host-start", "Start Hosted Run", "start");
    footer(ui, "host-port");
}

void direct_join_page(ViewBuilder& ui, const FrontPage& page) {
    const GubsyLobbyState& lobby = gubsy_get_lobby_state(*page.backend);
    frame(ui, "Direct Join (LAN / IP)", 700.0F, 430.0F);
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
    button(ui, "controller-icons", std::string{"Controller icons  ·  "} + controller_icon_name(), "controller-icons");
    footer(ui, "display");
}

void display_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Display Settings", 740.0F, 545.0F);
#ifdef __EMSCRIPTEN__
    button(ui,"window-mode",page.fullscreen ? "Exit fullscreen" : "Enter fullscreen","display:window-mode");
    button(ui,"render-scale", "Render resolution  ·  " + std::to_string(page.browser_render_percent) + "%", "display:render-scale");
    constexpr const char* caps[]{"Display refresh rate", "60 FPS", "120 FPS", "144 FPS"};
    button(ui,"frame-cap",std::string{"Frame limit  ·  "}+caps[std::clamp(page.frame_cap,0,3)],"display:frame-cap");
    button(ui,"show-fps",std::string{"Show FPS  ·  "}+(page.show_fps ? "On" : "Off"),"display:show-fps");
    button(ui,"auto-reports",std::string{"Automatic error reports  ·  "}+(page.auto_reports ? "On" : "Off"),"display:auto-reports");
    button(ui,"save-report","Save debug log","display:save-report");
    ui.label("card","reports-help","Reports send technical diagnostics, without player names or room codes.",40.0F,15.0F);
    footer(ui,"window-mode");
#else
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
#endif
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

void pause_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Paused", 950, 600);
    ui.layout_container("card", "pause-body", glayout::ContainerKind::Row,
        {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Fill, 1}, 24);
    ui.layout_container("pause-body", "pause-actions", glayout::ContainerKind::Column,
        {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Fill, 1}, 8);
    ui.button("pause-actions", "resume", "Resume", "resume", "menu", 46);
    if (page.allow_restart)
        ui.button("pause-actions", "restart", "Restart Run", "restart", "menu", 46);
    ui.button("pause-actions", "pause-settings", "Settings", "settings", "menu", 46);
    ui.button("pause-actions", "title", "Quit to Main Menu", "title", "menu", 46);
    control_reference(ui, "pause-body", page, active_profile_id(page));
    ui.focus_group("menu", "resume", "card");
}

void end_page(ViewBuilder& ui, const FrontPage& page, bool victory) {
    frame(ui, victory ? "Run Cleared" : "Run Over", 570.0F, 370.0F);
    ui.label("card", "end-note", page.allow_restart ?
             "Choose what happens next." : "Waiting for the host to restart.",
             42.0F, 17.0F);
    if (page.allow_restart) button(ui, "restart", "Start a New Run", "restart", 54.0F);
    button(ui, "title", "Return to Main Menu", "title", 50.0F);
    ui.focus_group("menu", page.allow_restart ? "restart" : "title", "card");
}

} // namespace

gview::View build_menu_page(const FrontPage& page, int width, int height,
                            int death_policy) {
    ViewBuilder ui("gauche-menu", width, height);
    switch (page.screen) {
    case MenuScreen::Main: main_page(ui); break;
    case MenuScreen::Dev:
        frame(ui,"Dev",560,320);
        button(ui,"worldgen","World Gen","worldgen",52);
        footer(ui,"worldgen"); break;
    case MenuScreen::Lobby: lobby_page(ui, page, death_policy); break;
    case MenuScreen::Rules: rules_page(ui, death_policy); break;
    case MenuScreen::Rooms: rooms_page(ui, page); break;
    case MenuScreen::Party: party_page(ui, page); break;
    case MenuScreen::Host: online_host_page(ui, page); break;
    case MenuScreen::DirectHost: direct_host_page(ui, page); break;
    case MenuScreen::DirectJoin: direct_join_page(ui, page); break;
    case MenuScreen::NetworkOptions: network_options_page(ui, page); break;
    case MenuScreen::Join: rooms_page(ui, page); break;
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
    case MenuScreen::Death: end_page(ui, page, false); break;
    case MenuScreen::Victory: end_page(ui, page, true); break;
    }
    if (!page.toast.empty()) {
        ui.label("card", "toast", page.toast, 28.0F, 14.0F,
                 gview::TextAlign::Center);
    }
    ui.layout("card").size.width.value = std::min(ui.layout("card").size.width.value,
        static_cast<float>(width) - 24);
    return ui.finish();
}
