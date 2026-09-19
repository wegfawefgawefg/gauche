#include "../app/options.hpp"
#include "../input/icon_set.hpp"
#include "../debug/playtest.hpp"
#include "../debug/worldgen.hpp"
#include "actions.hpp"
#include "settings.hpp"
#include <nlohmann/json.hpp>
#include "profiles.hpp"
#include "audio.hpp"
#include "../menu_shell.hpp"

#include <gubsy/settings/top_level_settings.hpp>
#include <gubsy/settings/audio.hpp>
#include <gubsy/input/profile_settings.hpp>
#include "src/engine_state.hpp"
#include "src/graphics.hpp"

#include <SDL3/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include "../browser/display.hpp"
#endif

#include <algorithm>
#include <charconv>
#include <cstdio>
#include <filesystem>
#include <string>

namespace {

std::uint16_t port_or_zero(std::string_view text) {
    int parsed = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), parsed);
    if (error != std::errc{} || end != text.data() + text.size() ||
        parsed < 1 || parsed > 65535) return 0;
    return static_cast<std::uint16_t>(parsed);
}

void leave_session(MenuShell& menu) {
    if (menu.rooms.active || menu.rooms.busy) { leave_room_session(menu); return; }
    if (gubsy_get_lobby_state(*menu.runtime).online) {
        std::string message;
        if (!gubsy_leave_lobby_room(*menu.runtime, message))
            menu.front.toast = message.empty() ? "Could not leave session" : message;
    }
    leave_network_game(*menu.network);
}

void back(MenuShell& menu) {
    FrontPage& page = menu.front;
    switch (page.screen) {
    case MenuScreen::Main: break;
    case MenuScreen::Dev: show_menu_screen(page,MenuScreen::Main); break;
    case MenuScreen::Rooms:
        if (menu.rooms.active || menu.rooms.busy) leave_room_session(menu);
        show_menu_screen(page, MenuScreen::Lobby); break;
    case MenuScreen::Party: leave_room_session(menu); break;
    case MenuScreen::Lobby:
        if (gubsy_get_lobby_state(*menu.runtime).online) leave_session(menu);
        show_menu_screen(page, MenuScreen::Main); break;
    case MenuScreen::DirectHost: case MenuScreen::DirectJoin:
        show_menu_screen(page, MenuScreen::NetworkOptions); break;
    case MenuScreen::Host:
        if (menu.rooms.busy) leave_room_session(menu);
        show_menu_screen(page, MenuScreen::Lobby); break;
    case MenuScreen::Rules: case MenuScreen::NetworkOptions: case MenuScreen::Join:
    case MenuScreen::Players: show_menu_screen(page, MenuScreen::Lobby); break;
    case MenuScreen::Player: show_menu_screen(page, MenuScreen::Players); break;
    case MenuScreen::Settings: show_menu_screen(page, page.settings_return); break;
    case MenuScreen::Display: case MenuScreen::Audio: case MenuScreen::Controls:
        show_menu_screen(page, MenuScreen::Settings); break;
    case MenuScreen::ProfileEditor: show_menu_screen(page, MenuScreen::Controls); break;
    case MenuScreen::Bindings: show_menu_screen(page, MenuScreen::ProfileEditor); break;
    case MenuScreen::BindDetail: show_menu_screen(page, MenuScreen::Bindings); break;
    case MenuScreen::BindChoices: show_menu_screen(page, MenuScreen::BindDetail); break;
    case MenuScreen::InputOptions: show_menu_screen(page, MenuScreen::ProfileEditor); break;
    case MenuScreen::Pause:
        menu.visible = menu.front_visible = false; break;
    case MenuScreen::Death: case MenuScreen::Victory: break;
    }
}

void display_action(std::string_view action) {
#ifdef __EMSCRIPTEN__
    if (action == "display:window-mode") EM_ASM({Module.toggleFullscreen();});
    if (action == "display:save-report") EM_ASM({Module.saveReport();});
#else
    (void)action;
#endif
}

void start_local(MenuShell& menu) {
    if (menu.network->role != NetRole::Solo) return;
    start_solo_run(*menu.solo_game, SDL_GetTicks() + 1, menu.death_policy);
    menu.solo_game->run.death_policy = menu.death_policy;
    menu.playing = true;
    menu.visible = menu.front_visible = false;
}

void pause_action(MenuShell& menu, std::string_view action) {
    if (action == "resume") {
        menu.visible = menu.front_visible = false;
    } else if (action == "restart") {
        if (menu.network->role == NetRole::Host)
            restart_host_run(*menu.network, SDL_GetTicks() + 1);
        else if (menu.network->role == NetRole::Solo) start_local(menu);
        menu.visible = menu.front_visible = false;
    } else if (action == "title") {
        leave_session(menu);
        *menu.solo_game = {};
        menu.playing = false;
        show_title_menu(menu);
    }
}

} // namespace

void initialize_menu_settings(MenuShell& menu) {
    EngineState& engine = gubsy_runtime_engine(*menu.runtime);
    FrontPage& page = menu.front;
    page.fullscreen = get_top_level_setting_string(engine.top_level_game_settings,
        "gubsy.video.window_mode", "windowed") == "fullscreen";
    const std::string mode = get_top_level_setting_string(engine.top_level_game_settings,
        "gubsy.video.window_mode", "windowed");
    page.window_mode = mode == "fullscreen" ? 2 : (mode == "borderless" ? 1 : 0);
    page.vsync = get_top_level_setting_int(engine.top_level_game_settings,
        "gubsy.video.vsync", 1) != 0;
#ifdef __EMSCRIPTEN__
    // RAF owns presentation. SDL's fallback V-sync would add a second 60 Hz wait.
    page.vsync = false;
    set_top_level_setting_int(engine.top_level_game_settings,"gubsy.video.vsync",0);
    const int saved_percent = get_top_level_setting_int(engine.top_level_game_settings,
        "teeming.video.browser_render_percent", 100);
    page.browser_render_percent = saved_percent == 50 || saved_percent == 75 ? saved_percent : 100;
    (void)sync_browser_render_resolution(menu);
#endif
    // Apply the saved preference to this newly created renderer, not only when toggled.
    const auto frame = gubsy_get_frame(*menu.runtime);
    if (frame.renderer && !tr::set_vsync(frame.renderer, page.vsync ? 1 : 0))
        std::fprintf(stderr, "Could not apply V-sync preference: %s\n", SDL_GetError());
    page.show_fps = get_top_level_setting_int(engine.top_level_game_settings,
        "gubsy.video.show_fps", 0) != 0;
    page.master_volume = engine.audio_settings.vol_master;
    page.music_volume = engine.audio_settings.vol_music;
    page.sfx_volume = engine.audio_settings.vol_sfx;
    for (const BindsProfile& binds : gubsy_get_binds_profiles(*menu.runtime))
        ensure_input_pair(engine, binds);
}

void apply_menu_action(MenuShell& menu, std::string_view action) {
    // Explicit values also support browser inspection and scripted checks.
    if (action.starts_with("setting:")) {
        const auto separator = action.find(':', 8);
        if (separator == std::string_view::npos) return;
        const auto data = nlohmann::json::parse(action.substr(separator + 1), nullptr, false);
        const auto key = action.substr(0, separator);
        if (data.is_boolean()) apply_menu_setting(menu, key, data.get<bool>());
        else if (data.is_number()) apply_menu_setting(menu, key, data.get<double>());
        else if (data.is_string()) apply_menu_setting(menu, key, data.get<std::string>());
        return;
    }
    if (action.empty()) return;
    if (room_action(menu, action)) { menu.front.dirty = true; return; }
    FrontPage& page = menu.front;
    if (page.screen == MenuScreen::ProfileEditor && !save_profile_name(page)) return;
    if (action == "back") { menu_back_sound(page); back(menu); return; }
    if (action == "join" || action == "rooms") {
        show_menu_screen(page, MenuScreen::Rooms);
        room_action(menu, "room:browse"); return;
    }
    if (action == "host") page.room_status.clear();
    if (action == "play") { show_menu_screen(page, MenuScreen::Lobby); return; }
    if (TEEMING_DEV_MODE && action == "dev") { show_menu_screen(page,MenuScreen::Dev); return; }
    if (TEEMING_DEV_MODE && action == "worldgen") { worldgen_viewer().open_requested=true; return; }
    if (action == "quick") { start_local(menu); return; }
    if (action == "quit") { menu.quit_requested = true; return; }
    if (action == "resume" || action == "restart" || action == "title") {
        pause_action(menu, action); return;
    }
    if (action == "settings") {
        page.settings_return = page.screen;
        show_menu_screen(page, MenuScreen::Settings); return;
    }
    constexpr struct { std::string_view action; MenuScreen screen; } screens[]{
        {"players", MenuScreen::Players}, {"rules", MenuScreen::Rules},
        {"rooms", MenuScreen::Rooms}, {"party", MenuScreen::Party},
        {"host", MenuScreen::Host}, {"join", MenuScreen::Rooms},
        {"direct-host", MenuScreen::DirectHost}, {"direct-join", MenuScreen::DirectJoin},
        {"network-options", MenuScreen::NetworkOptions},
        {"display", MenuScreen::Display}, {"audio", MenuScreen::Audio},
        {"controls", MenuScreen::Controls},
    };
    for (const auto& target : screens)
        if (action == target.action) { show_menu_screen(page, target.screen); return; }
    if (action.starts_with("policy:")) {
        constexpr DeathPolicy choices[]{DeathPolicy::NextFloor, DeathPolicy::Entrance,
                                        DeathPolicy::NoRespawn};
        const int choice = action.back() - '0';
        if (choice >= 0 && choice < 3) menu.death_policy = choices[choice];
        page.dirty = true; return;
    }
    if (action == "start") {
        if (menu.rooms.active && !menu.network->match_started) {
            show_menu_screen(page, MenuScreen::Party); return;
        }
        std::string message;
        if (!gubsy_start_lobby_game(*menu.runtime, message))
            page.toast = message.empty() ? "Could not start run" : message;
        page.dirty = true; return;
    }
    if (action == "host:open") {
        const auto port = port_or_zero(page.host_port);
        std::string message;
        if (port == 0 || !gubsy_host_lobby_direct(*menu.runtime, port, message))
            page.toast = port == 0 ? "Enter a valid UDP port" : message;
        else show_menu_screen(page, MenuScreen::Lobby);
        page.dirty = true; return;
    }
    if (action == "join:connect") {
        const auto port = port_or_zero(page.join_port);
        std::string message;
        if (port == 0 || page.join_host.empty() ||
            !gubsy_join_lobby_direct(*menu.runtime, page.join_host, port, message))
            page.toast = port == 0 ? "Enter a valid UDP port" : message;
        else show_menu_screen(page, MenuScreen::Lobby);
        page.dirty = true; return;
    }
    if (action == "leave") {
        leave_session(menu); page.dirty = true; return;
    }
    if (action.starts_with("player:")) {
        const std::string value{action.substr(7)};
        if (value.starts_with("profile:")) {
            const int id = std::stoi(value.substr(8));
            (void)gubsy_set_lobby_player_binds_profile(*menu.runtime,
                page.selected_player, id);
            (void)gubsy_set_lobby_player_input_settings_profile(*menu.runtime,
                page.selected_player, id);
        } else if (value == "keyboard") {
            gubsy_toggle_lobby_player_device(*menu.runtime, page.selected_player,
                {InputSourceType::Keyboard, 0});
            gubsy_toggle_lobby_player_device(*menu.runtime, page.selected_player,
                {InputSourceType::Mouse, 0});
        } else if (value.starts_with("pad:")) {
            gubsy_toggle_lobby_player_device(*menu.runtime, page.selected_player,
                {InputSourceType::Gamepad, std::stoi(value.substr(4))});
        } else {
            page.selected_player = std::stoi(value);
            show_menu_screen(page, MenuScreen::Player);
        }
        page.dirty = true; return;
    }
    if (profile_action(page, action)) return;
    if (binding_action(page, action)) return;
    if (action == "input-options") {
        show_menu_screen(page, MenuScreen::InputOptions); return;
    }
    if (action.starts_with("display:")) { display_action(action); return; }

}
