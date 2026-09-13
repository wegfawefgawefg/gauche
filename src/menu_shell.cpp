#include "menu_shell.hpp"
#include "input.hpp"

#include <gubsy/lobby/config.hpp>

#include <array>
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
    if (menu.network->role == NetRole::Client && !menu.network->ready) return;
    if (menu.network->role == NetRole::Solo) {
        start_run(*menu.solo_game, SDL_GetTicks() + 1);
        menu.solo_game->run.death_policy = menu.death_policy;
    }
    menu.playing = true;
    menu.visible = false;
    gubsy_clear_menu_stack(*menu.runtime);
}

void quit_game(void* data, std::int32_t) {
    static_cast<MenuShell*>(data)->quit_requested = true;
}

void resume_game(void* data, std::int32_t) {
    auto& menu = *static_cast<MenuShell*>(data);
    gubsy_close_in_game_menu(*menu.runtime);
    menu.visible = false;
}

void restart_game(void* data, std::int32_t) {
    auto& menu = *static_cast<MenuShell*>(data);
    if (menu.network->role == NetRole::Client) return;
    if (menu.network->role == NetRole::Host)
        restart_host_run(*menu.network, SDL_GetTicks() + 1);
    else {
        start_run(*menu.solo_game, SDL_GetTicks() + 1);
        menu.solo_game->run.death_policy = menu.death_policy;
    }
    gubsy_close_in_game_menu(*menu.runtime);
    menu.visible = false;
    menu.playing = true;
}

void back_to_title(void* data, std::int32_t) {
    auto& menu = *static_cast<MenuShell*>(data);
    if (gubsy_get_lobby_state(*menu.runtime).online) {
        std::string message;
        (void)gubsy_leave_lobby_room(*menu.runtime, message);
    }
    leave_network_game(*menu.network);
    *menu.solo_game = {};
    menu.playing = false;
    menu.visible = gubsy_show_main_menu(*menu.runtime);
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
    lobby.max_players = 4;
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
    if (lobby.max_players <= 4) return true;
    message = "Gauche supports at most four players";
    return false;
}

void sync_direct_members(MenuShell& menu) {
    const NetSession& network = *menu.network;
    if (network.role == NetRole::Solo ||
        !gubsy_get_lobby_state(*menu.runtime).online) return;
    std::vector<MatchmakingMember> members;
    for (int owner = 0; owner < 4; ++owner) {
        if (owner == network.local_owner) continue;
        const bool connected = network.role == NetRole::Host ?
            network.peers[static_cast<std::size_t>(owner)].connected :
            network.ready && network.rollback.game.run.online[static_cast<std::size_t>(owner)];
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
    register_game_bindings(runtime);
    GubsyMainMenuCommands main_commands;
    main_commands.start_game = gubsy_register_menu_command(runtime, start_game, &menu);
    main_commands.quit = gubsy_register_menu_command(runtime, quit_game, &menu);
    gubsy_set_main_menu_commands(runtime, main_commands);
    GubsyInGameMenuCommands game_commands;
    game_commands.resume = gubsy_register_menu_command(runtime, resume_game, &menu);
    game_commands.restart_run = gubsy_register_menu_command(runtime, restart_game, &menu);
    game_commands.quit_to_main_menu =
        gubsy_register_menu_command(runtime, back_to_title, &menu);
    menu.game_commands = game_commands;
    gubsy_set_in_game_menu_commands(runtime, game_commands);
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
}

void show_title_menu(MenuShell& menu) {
    menu.visible = gubsy_show_main_menu(*menu.runtime);
}

void open_game_menu(MenuShell& menu) {
    if (menu.visible || !menu.playing) return;
    GubsyInGameMenuCommands commands = menu.game_commands;
    if (menu.network->role == NetRole::Client) commands.restart_run = kMenuIdInvalid;
    gubsy_set_in_game_menu_commands(*menu.runtime, commands);
    menu.visible = gubsy_open_in_game_menu(*menu.runtime);
}

void update_menu_shell(MenuShell& menu, MenuInputState input, float dt,
                       int width, int height) {
    sync_direct_members(menu);
    if (menu.network->role == NetRole::Client && menu.network->ready && menu.visible) {
        const GubsyLobbyState& lobby = gubsy_get_lobby_state(*menu.runtime);
        if (lobby.direct_join_pending)
            gubsy_confirm_lobby_direct_join(*menu.runtime, "Joined Gauche host");
        if (menu.network->host_tick > 0)
            gubsy_set_lobby_session_phase(*menu.runtime, "in_game");
    }
    if (menu.network->role == NetRole::Client && !menu.network->ready &&
        menu.network->pump_tick > 360 && menu.visible &&
        gubsy_get_lobby_state(*menu.runtime).direct_join_pending) {
        gubsy_fail_lobby_direct_join(*menu.runtime, "Host did not answer");
        leave_network_game(*menu.network);
    }
    if (menu.visible) {
        gubsy_set_menu_input(*menu.runtime, input);
        gubsy_update_menu(*menu.runtime, dt, width, height);
    } else gubsy_update_runtime(*menu.runtime, dt);
}

void render_menu_shell(MenuShell& menu, SDL_Renderer* renderer, int width, int height) {
    if (menu.visible) gubsy_render_menu(*menu.runtime, renderer, width, height);
    gubsy_render_alerts(*menu.runtime, renderer, width);
}
