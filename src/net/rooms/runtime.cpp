#include "runtime.hpp"
#include "names.hpp"
#include "../../menu_shell.hpp"
#include "../../menu/actions.hpp"
#include "../party.hpp"
#include <gubsy/lobby/state.hpp>

#include <algorithm>
#include <chrono>
#include <cctype>
#include <filesystem>
#include <fstream>

namespace {

void submit(RoomRuntime& room, RoomRequest request) {
    room.busy = true;
    room.pending = std::async(std::launch::async, [request = std::move(request)] {
        return perform_room_request(request);
    });
}

RoomRequest request_for(const RoomRuntime& room, RoomOperation operation) {
    RoomRequest request;
    request.operation = operation;
    request.url = room.url; request.code = room.code; request.name = room.name;
    request.member = room.member; request.secret = room.secret; request.token = room.token;
    request.room = room.metadata; request.host = room.host;
    return request;
}

void save_preferences(const MenuShell& menu) {
    const auto path = std::filesystem::path(menu.identity_path).parent_path() / "rooms.json";
    std::ofstream output(path);
    output << nlohmann::json{{"url", menu.front.room_url}, {"name", menu.front.player_name},
        {"room", menu.front.room_name}}.dump(2) << '\n';
}

void show_party(MenuShell& menu) {
    menu.front.room_active = true;
    auto& lobby = gubsy_lobby_state(gubsy_runtime_engine(*menu.runtime));
    lobby.online = true; lobby.is_host = menu.rooms.host;
    // DIRECTORY: Gauche owns asynchronous room traffic; Gubsy keeps local player/config UI.
    lobby.room_code.clear(); lobby.member_id.clear(); lobby.host_secret.clear();
    lobby.direct_join_pending = false;
    lobby.advertised_endpoint = "Room " + menu.rooms.code;
    lobby.status_message = "Room " + menu.rooms.code;
    show_menu_screen(menu.front, MenuScreen::Party);
}

void configure_traversal(MenuShell& menu, const RoomResult& result) {
    auto& transport = menu.network->traversal;
    transport = {};
    transport.phase = TraversalPhase::Punch;
    transport.host = menu.rooms.host;
    transport.room = menu.rooms.code;
    transport.host_secret = menu.rooms.secret;
    transport.punch_server = result.punch;
    transport.relay_server = result.relay;
    transport.force_relay = menu.front.force_relay || result.punch.port == 0;
    if (!transport.host) {
        transport.attempt = result.attempt.join_attempt_id;
        transport.punch_secret = result.attempt.punch_secret;
        transport.allocation = result.attempt.relay_allocation_id;
        transport.relay_secret = result.attempt.relay_secret;
    }
    menu.network->match_started = false;
    menu.network->party_ready = transport.host;
}

void complete_request(MenuShell& menu, RoomResult result) {
    auto& room = menu.rooms;
    if (!result.okay) {
        room.next_browse_ms = network_clock_ms() + 5000;
        menu.front.room_status = result.error.empty() ? "Room request failed" : result.error;
        if (result.operation == RoomOperation::Create || result.operation == RoomOperation::Attempt)
            leave_network_game(*menu.network);
        if (result.operation == RoomOperation::Finalize) { room.token.clear(); leave_room_session(menu); }
        if (result.operation == RoomOperation::Leave) {
            room.active = false; room.member.clear(); room.code.clear(); room.secret.clear();
        }
        return;
    }
    switch (result.operation) {
    case RoomOperation::Browse:
        menu.front.rooms = std::move(result.rooms);
        menu.front.room_page = 0;
        room.next_browse_ms = network_clock_ms() + 5000;
        menu.front.room_status = menu.front.rooms.empty() ? "No public Gauche rooms" : "Choose a room or enter its code";
        break;
    case RoomOperation::Create:
        room.active = true;
        room.code = result.created.room_code;
        room.member = result.created.member_id;
        room.secret = result.created.host_secret;
        room.metadata = result.room;
        configure_traversal(menu, result);
        menu.front.room_status = "Room open. Share the code with your friends.";
        show_party(menu);
        break;
    case RoomOperation::Attempt: {
        std::string error;
        if (!join_game(*menu.network, "127.0.0.1", 1,
                load_or_create_identity(menu.identity_path), error)) {
            menu.front.room_status = error; return;
        }
        room.active = true;
        room.metadata = result.room;
        room.token = result.attempt.join_token;
        configure_traversal(menu, result);
        menu.front.room_status = "Connecting to room " + room.code;
        show_party(menu);
        break;
    }
    case RoomOperation::Finalize:
        room.member = result.member;
        room.token.clear();
        menu.front.room_status = menu.network->match_started ? "Joined the run in progress." : "Connected. Ready up when you want to start.";
        [[fallthrough]];
    case RoomOperation::Heartbeat:
        if (!result.room.room_code.empty()) menu.front.room_members = result.room.members;
        break;
    case RoomOperation::Leave:
        room.active = false;
        room.code.clear(); room.member.clear(); room.secret.clear(); room.token.clear();
        break;
    }
    room.next_heartbeat_ms = network_clock_ms() + 2000;
}

} // namespace

void load_room_preferences(MenuShell& menu) {
    std::ifstream input(std::filesystem::path(menu.identity_path).parent_path() / "rooms.json");
    if (input) {
        auto data = nlohmann::json::parse(input, nullptr, false);
        if (data.is_object()) {
            if (data.contains("url") && data["url"].is_string()) menu.front.room_url = data["url"];
            if (data.contains("name") && data["name"].is_string()) menu.front.player_name = data["name"];
            if (data.contains("room") && data["room"].is_string()) menu.front.room_name = data["room"];
        }
    }
    if (menu.front.player_name.empty() || menu.front.player_name == "Player") {
        menu.front.player_name = random_room_player_name();
        save_preferences(menu);
    }
}

void shutdown_room_session(MenuShell& menu) {
    auto& room = menu.rooms;
    // EXIT: Finish the bounded HTTP request, then release any membership it created.
    if (room.busy) {
        const auto result = room.pending.get();
        room.busy = false;
        if (result.okay && result.operation == RoomOperation::Create) {
            room.code = result.created.room_code;
            room.member = result.created.member_id;
            room.secret = result.created.host_secret;
        } else if (result.okay && result.operation == RoomOperation::Finalize) {
            room.member = result.member;
        } else if (result.okay && result.operation == RoomOperation::Leave) room.member.clear();
    }
    if (!room.member.empty()) (void)perform_room_request(request_for(room, RoomOperation::Leave));
    room.active = false;
    room.member.clear(); room.secret.clear(); room.token.clear();
}

bool room_action(MenuShell& menu, std::string_view action) {
    auto& room = menu.rooms;
    auto& page = menu.front;
    if (!action.starts_with("room:")) return false;
    if (action == "room:shuffle-name") {
        page.player_name = random_room_player_name(); save_preferences(menu);
        page.dirty = true; return true;
    }
    if (action == "room:leave") { leave_room_session(menu); return true; }
    if (action == "room:next" || action == "room:previous") {
        const int last = std::max(0, (static_cast<int>(page.rooms.size()) - 1) / 4);
        page.room_page = std::clamp(page.room_page + (action == "room:next" ? 1 : -1), 0, last);
        page.dirty = true; return true;
    }
    if (room.busy && action != "room:ready" && action != "room:start") { page.room_status = "Waiting for the room service…"; return true; }
    if (action == "room:ready") {
        menu.network->party_ready = !menu.network->party_ready;
        send_party_state(*menu.network); return true;
    }
    if (action == "room:start") {
        if (start_network_party(*menu.network)) apply_menu_action(menu, "start");
        else page.room_status = "Wait for everyone to finish joining and ready up";
        return true;
    }
    if (action == "room:relay") { page.force_relay = !page.force_relay; page.dirty = true; return true; }
    if (room.active) { page.room_status = "Leave the current room first"; return true; }
    room.url = page.room_url;
    room.name = page.player_name.empty() ? "Player" : page.player_name.substr(0, 40);
    room.cancel = false;
    save_preferences(menu);
    if (action == "room:browse") {
        submit(room, request_for(room, RoomOperation::Browse));
        show_menu_screen(page, MenuScreen::Rooms);
    } else if (action == "room:create") {
        if (menu.network->role != NetRole::Solo) { page.room_status = "Leave the direct session first"; return true; }
        std::string error;
        if (!host_game(*menu.network, 0, SDL_GetTicks() + 1, menu.death_policy, error)) {
            page.room_status = error; return true;
        }
        menu.network->match_started = false;
        room.host = true;
        room.metadata = {};
        room.metadata.host_name = room.name;
        room.metadata.session_name = page.room_name.substr(0, 64);
        room.metadata.privacy = 1;
        room.metadata.max_players = 4;
        room.metadata.contract.game_version = std::to_string(gameplay_version);
        room.metadata.contract.net_protocol = "gauche-" + std::to_string(wire_version);
        room.metadata.contract.allow_live_mod_reload = false;
        room.metadata.contract.game_config = {{"death_policy", static_cast<int>(menu.death_policy)}};
        submit(room, request_for(room, RoomOperation::Create));
    } else if (action == "room:join" || action.starts_with("room:code:")) {
        if (menu.network->role != NetRole::Solo) { page.room_status = "Leave the direct session first"; return true; }
        room.host = false;
        room.code = action == "room:join" ? page.room_code : std::string(action.substr(10));
        std::erase_if(room.code, [](unsigned char c) { return std::isspace(c) != 0; });
        std::transform(room.code.begin(), room.code.end(), room.code.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
        if (room.code.size() != 6 || !std::all_of(room.code.begin(), room.code.end(), [](unsigned char c) { return std::isalnum(c); })) {
            page.room_status = "Enter the six-character room code"; return true;
        }
        submit(room, request_for(room, RoomOperation::Attempt));
    }
    page.room_status = "Contacting room service…";
    page.dirty = true;
    return true;
}

void leave_room_session(MenuShell& menu) {
    auto& room = menu.rooms;
    room.cancel = true;
    menu.front.room_active = false;
    leave_network_game(*menu.network);
    auto& lobby = gubsy_lobby_state(gubsy_runtime_engine(*menu.runtime));
    lobby.online = false; lobby.is_host = false; lobby.game_members.clear();
    lobby.advertised_endpoint.clear(); lobby.status_message.clear();
    menu.playing = false;
    show_menu_screen(menu.front, MenuScreen::Rooms);
}

void update_room_session(MenuShell& menu) {
    auto& room = menu.rooms;
    auto& page = menu.front;
    if (room.busy && room.pending.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
        room.busy = false;
        auto result = room.pending.get();
        // CANCEL: A late creation still needs its server room removed, not shown as a new party.
        if (room.cancel && result.operation == RoomOperation::Create && result.okay) {
            room.active = true; room.code = result.created.room_code;
            room.member = result.created.member_id; room.secret = result.created.host_secret;
        } else if (room.cancel && result.operation == RoomOperation::Finalize && result.okay) {
            room.member = result.member;
        } else if (!room.cancel || result.operation == RoomOperation::Leave) complete_request(menu, std::move(result));
        page.dirty = true;
    }
    page.room_busy = room.busy;
    page.party_host = menu.network->role == NetRole::Host;
    page.party_ready = menu.network->party_ready;
    page.party_ready_mask = menu.network->party_ready_mask;
    page.party_code = room.code;
    page.connection_status = traversal_status(*menu.network);
    if (room.active && !room.host && !menu.network->ready && !menu.network->status.empty())
        page.room_status = menu.network->status;
    if (room.busy) return;
    if (room.cancel) {
        if (room.active && !room.member.empty()) submit(room, request_for(room, RoomOperation::Leave));
        else { room.active = false; room.code.clear(); }
        return;
    }
    if (!room.active) {
        if (page.screen == MenuScreen::Rooms && menu.front_visible &&
            network_clock_ms() >= room.next_browse_ms) {
            room.next_browse_ms = network_clock_ms() + 5000;
            room_action(menu, "room:browse");
        }
        return;
    }
    if (!room.host && menu.network->ready && room.member.empty() && !room.token.empty()) {
        submit(room, request_for(room, RoomOperation::Finalize)); return;
    }
    if (!room.member.empty() && network_clock_ms() >= room.next_heartbeat_ms) {
        room.next_heartbeat_ms = network_clock_ms() + 2000;
        room.metadata.contract.session_phase = menu.network->match_started ? "in_game" : "lobby";
        submit(room, request_for(room, RoomOperation::Heartbeat));
    }
    if (!room.host && menu.network->ready && menu.network->match_started && !menu.playing) {
        menu.playing = true; menu.visible = menu.front_visible = false;
    }
}
