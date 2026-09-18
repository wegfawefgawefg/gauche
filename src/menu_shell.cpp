#include "debug/playtest.hpp"
#include "menu_shell.hpp"
#include "input.hpp"
#include "menu/actions.hpp"

#include <gubsy/lobby/config.hpp>
#include <gubsy/lobby/state.hpp>

#include <array>
#include <algorithm>
#include <cstring>
#include <utility>

namespace {

struct PolicyOption {
    const char* id;
    const char* label;
    const char* description;
    DeathPolicy policy;
};

constexpr std::array<PolicyOption, 3> policies{{
    {"next-floor", "Next Floor", "Dead players return on the next floor.",
     DeathPolicy::NextFloor},
    {"entrance", "Entrance", "Dead players return at the entrance after a delay.",
     DeathPolicy::Entrance},
    {"no-respawn", "No Respawn", "Death lasts for this run.",
     DeathPolicy::NoRespawn},
}};

int policy_index(DeathPolicy policy) {
    for (int index = 0; index < static_cast<int>(policies.size()); ++index)
        if (policies[static_cast<std::size_t>(index)].policy == policy) return index;
    return 0;
}

void start_game(void* data, std::int32_t) {
    auto& menu = *static_cast<MenuShell*>(data);
    if (menu.network->role != NetRole::Solo && !menu.network->match_started) return;
    if (menu.network->role == NetRole::Client && !menu.network->ready) return;
    if (menu.network->role == NetRole::Solo) {
        start_solo_run(*menu.solo_game, SDL_GetTicks() + 1, menu.death_policy);
        menu.solo_game->run.death_policy = menu.death_policy;
    }
    menu.playing = true;
    menu.visible = false;
    menu.front_visible = false;
    gubsy_clear_menu_stack(*menu.runtime);
}

void quit_game(void* data, std::int32_t) {
    static_cast<MenuShell*>(data)->quit_requested = true;
}

GubsyLobbyHostResult host_direct(void* data, const GubsyLobbyState&,
                                 std::uint16_t requested_port) {
    auto& menu = *static_cast<MenuShell*>(data);
    if (menu.network->role != NetRole::Solo)
        return {false, "Leave the current session first", {}};
    const std::uint16_t port = requested_port == 0 ? 39000 : requested_port;
    std::string error;
    if (!host_game(*menu.network, port, SDL_GetTicks() + 1,
                   menu.death_policy, error)) return {false, error, {}};
    return {true, "Hosting direct UDP", "localhost:" + std::to_string(port)};
}

GubsyLobbyJoinResult join_direct(void* data, const GubsyLobbyState&,
                                 const char* host, std::uint16_t port) {
    auto& menu = *static_cast<MenuShell*>(data);
    if (menu.network->role != NetRole::Solo)
        return {false, false, "Leave the current session first"};
    std::string error;
    if (!join_game(*menu.network, host, port,
                   load_or_create_identity(menu.identity_path), error))
        return {false, false, error};
    return {true, true, "Connecting to host"};
}

GubsyLobbyLeaveResult leave_direct(void* data, const GubsyLobbyState&) {
    auto& menu = *static_cast<MenuShell*>(data);
    leave_network_game(*menu.network);
    menu.playing = false;
    return {true, "Left direct game"};
}

void build_policy_rows(void* data, const GubsyLobbyState&,
                       std::vector<GubsyLobbyConfigRow>& out) {
    auto& menu = *static_cast<MenuShell*>(data);
    GubsyLobbyConfigRow row;
    row.key = "death_policy";
    row.label = "Death Policy";
    row.description = "Choose when a dead player can return.";
    row.selected_option = policy_index(menu.death_policy);
    for (const PolicyOption& option : policies)
        row.options.push_back({option.id, option.label, option.description});
    out.push_back(std::move(row));
}

void ensure_lobby_defaults(void* data, GubsyLobbyState& lobby) {
    auto& menu = *static_cast<MenuShell*>(data);
    if (menu.lobby_configured) return;
    lobby.max_players = menu.network->admission_limit;
    menu.lobby_configured = true;
}

bool set_policy(void* data, GubsyLobbyState&, const char* key, int, int option) {
    if (key == nullptr || std::strcmp(key, "death_policy") != 0 ||
        option < 0 || option >= static_cast<int>(policies.size())) return false;
    static_cast<MenuShell*>(data)->death_policy = policies[static_cast<std::size_t>(option)].policy;
    return true;
}

nlohmann::json serialize_policy(void* data, const GubsyLobbyState&) {
    const auto policy = static_cast<MenuShell*>(data)->death_policy;
    return {{"death_policy", policies[static_cast<std::size_t>(policy_index(policy))].id}};
}

bool validate_policy(void*, const GubsyLobbyState& lobby, std::string& message) {
    if (lobby.max_players > 0) return true;
    message = "Player capacity must be positive";
    return false;
}

void sync_direct_members(MenuShell& menu) {
    const NetSession& network = *menu.network;
    if (network.role == NetRole::Solo ||
        !gubsy_get_lobby_state(*menu.runtime).online) return;
    std::vector<MatchmakingMember> members;
    for (const auto& [owner, participant] : network.rollback.game.players) {
        if (owner == network.local_owner) continue;
        const bool connected = network.role == NetRole::Host ?
            network.peers.contains(owner) && network.peers.at(owner).connected :
            network.ready && player_state(network.rollback.game, owner).online;
        if (!connected) continue;
        MatchmakingMember member;
        member.member_id = "gauche-player-" + std::to_string(owner + 1);
        member.display_name = owner == 0 ? "Host" : "Player " + std::to_string(owner + 1);
        member.client_label = "Direct UDP";
        member.is_host = owner == 0;
        members.push_back(std::move(member));
    }
    gubsy_set_lobby_direct_members(*menu.runtime, members, false);
}

bool validate_remote_policy(void*, const GubsyLobbyState&,
                            const SessionContract& remote, std::string& message) {
    if (!remote.game_config.is_object()) {
        message = "Host death policy is missing";
        return false;
    }
    const std::string id = remote.game_config.value("death_policy", "");
    for (const PolicyOption& option : policies)
        if (id == option.id) return true;
    message = "Host death policy is unknown";
    return false;
}

bool apply_remote_policy(void* data, GubsyLobbyState& lobby,
                         const SessionContract& remote, std::string& message) {
    if (!validate_remote_policy(data, lobby, remote, message)) return false;
    const std::string id = remote.game_config.value("death_policy", "");
    for (const PolicyOption& option : policies)
        if (id == option.id) static_cast<MenuShell*>(data)->death_policy = option.policy;
    return true;
}

} // namespace

void init_menu_shell(MenuShell& menu, GubsyRuntime& runtime, Game& game,
                     NetSession& network, DeathPolicy policy,
                     const std::string& identity_path) {
    menu.runtime = &runtime;
    menu.solo_game = &game;
    menu.network = &network;
    menu.death_policy = policy;
    menu.identity_path = identity_path;
    load_room_preferences(menu);
    register_game_bindings(runtime);
    gubsy_lobby_ensure_ready(gubsy_runtime_engine(runtime));
    // What if a gamepad was opened first? Keep keyboard and mouse assigned too.
    for (InputSourceType type : {InputSourceType::Keyboard, InputSourceType::Mouse}) {
        const auto& players = gubsy_get_lobby_state(runtime).local_players;
        if (players.empty()) break;
        const auto& devices = players.front().devices;
        if (std::none_of(devices.begin(), devices.end(), [type](const auto& device) {
                return device.type == type;
            })) gubsy_toggle_lobby_player_device(runtime, 0, {type, 0});
    }
    GubsyMainMenuCommands main_commands;
    main_commands.start_game = gubsy_register_menu_command(runtime, start_game, &menu);
    main_commands.quit = gubsy_register_menu_command(runtime, quit_game, &menu);
    gubsy_set_main_menu_commands(runtime, main_commands);
    GubsyLobbyCommands lobby_commands;
    lobby_commands.host = host_direct;
    lobby_commands.host_user_data = &menu;
    lobby_commands.join = join_direct;
    lobby_commands.join_user_data = &menu;
    lobby_commands.leave = leave_direct;
    lobby_commands.leave_user_data = &menu;
    gubsy_set_lobby_commands(runtime, lobby_commands);
    GubsyLobbyConfigProvider provider;
    provider.user_data = &menu;
    provider.ensure_defaults = ensure_lobby_defaults;
    provider.build_rows = build_policy_rows;
    provider.set_option = set_policy;
    provider.serialize = serialize_policy;
    provider.validate = validate_policy;
    provider.validate_remote = validate_remote_policy;
    provider.apply_remote = apply_remote_policy;
    gubsy_set_lobby_config_provider(runtime, provider);
    initialize_menu_settings(menu);
}

void show_title_menu(MenuShell& menu) {
    gubsy_clear_menu_stack(*menu.runtime);
    show_menu_screen(menu.front, MenuScreen::Main);
    menu.front_visible = true;
    menu.visible = true;
}

void open_game_menu(MenuShell& menu) {
    if (menu.visible || !menu.playing) return;
    menu.front.allow_restart = menu.network->role != NetRole::Client;
    show_menu_screen(menu.front, MenuScreen::Pause);
    menu.front_visible = true;
    menu.visible = true;
}

void open_end_menu(MenuShell& menu, bool victory) {
    if (menu.visible || !menu.playing) return;
    menu.front.allow_restart = menu.network->role != NetRole::Client;
    show_menu_screen(menu.front, victory ? MenuScreen::Victory : MenuScreen::Death);
    menu.front_visible = true;
    menu.visible = true;
}

void sync_run_end_menu(MenuShell& menu) {
    const Game& game = menu.network->role == NetRole::Solo ? *menu.solo_game : menu.network->rollback.game;
    const bool ended = game.game_over || game.run.phase == RunPhase::Won;
    const bool end_screen = menu.front.screen == MenuScreen::Death || menu.front.screen == MenuScreen::Victory;
    if (menu.playing && end_screen && !ended) {
        menu.visible = menu.front_visible = false;
        menu.front.dirty = true;
    }
    if (menu.playing && !menu.visible && ended && network_end_confirmed(*menu.network)) {
        network_event(*menu.network, "run_end_menu", menu.network->local_owner, game.tick);
        open_end_menu(menu, game.run.phase == RunPhase::Won);
    }
}

void update_menu_shell(MenuShell& menu, MenuInputState input, float dt,
                       int width, int height) {
    update_room_session(menu);
    sync_direct_members(menu);
    if (menu.network->role == NetRole::Client && menu.network->ready && menu.visible) {
        const GubsyLobbyState& lobby = gubsy_get_lobby_state(*menu.runtime);
        if (lobby.direct_join_pending)
            gubsy_confirm_lobby_direct_join(*menu.runtime, "Joined Gauche host");
        if (menu.network->match_started && menu.network->host_tick > 0)
            gubsy_set_lobby_session_phase(*menu.runtime, "in_game");
    }
    if (menu.network->role == NetRole::Client && !menu.network->ready &&
        menu.network->now_ms - menu.network->started_ms > 6000 && menu.visible &&
        gubsy_get_lobby_state(*menu.runtime).direct_join_pending) {
        gubsy_fail_lobby_direct_join(*menu.runtime, "Host did not answer");
        leave_network_game(*menu.network);
    }
    if (menu.front_visible) {
        gubsy_update_runtime(*menu.runtime, dt);
        if (!init_front_page(menu.front, *menu.runtime,
                             gubsy_get_frame(*menu.runtime).renderer)) {
            menu.quit_requested = true;
            return;
        }
        if (input.back && !menu.front.capturing_bind &&
            !menu.front.text_input_active) apply_menu_action(menu, "back");
        else apply_menu_action(menu, update_front_page(menu.front, input, width, height,
                                  policy_index(menu.death_policy)));
        return;
    }
    gubsy_update_runtime(*menu.runtime, dt);
}

void render_menu_shell(MenuShell& menu, tr::Renderer* renderer, int width, int) {
    if (menu.front_visible) render_front_page(menu.front);
    gubsy_render_alerts(*menu.runtime, renderer, width);
}

bool process_menu_shell_event(MenuShell& menu, const SDL_Event& event,
                              const GubsyFrame& frame) {
    return menu.front_visible && front_page_event(menu.front, event, frame);
}

void shutdown_menu_shell(MenuShell& menu) {
    leave_network_game(*menu.network);
    shutdown_room_session(menu);
    shutdown_front_page(menu.front);
}
