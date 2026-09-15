#pragma once

#include "matchmaking.hpp"
#include "../traversal.hpp"

#include <future>

struct MenuShell;

enum class RoomOperation { Browse, Create, Attempt, Finalize, Heartbeat, Leave };

struct RoomRequest {
    RoomOperation operation = RoomOperation::Browse;
    std::string url, code, name, member, secret, token;
    MatchmakingRoom room;
    bool host = false;
};

struct RoomResult {
    RoomOperation operation = RoomOperation::Browse;
    bool okay = false;
    std::string error, member;
    MatchmakingCreateResult created;
    MatchmakingJoinAttemptResult attempt;
    MatchmakingRoom room;
    std::vector<MatchmakingRoom> rooms;
    NetEndpoint punch{}, relay{};
};

struct RoomRuntime {
    std::future<RoomResult> pending;
    bool busy = false, active = false, host = false, cancel = false;
    std::string url, code, member, secret, token, name;
    MatchmakingRoom metadata;
    std::uint64_t next_heartbeat_ms = 0;
};

RoomResult perform_room_request(const RoomRequest& request);
bool room_action(MenuShell& menu, std::string_view action);
void update_room_session(MenuShell& menu);
void leave_room_session(MenuShell& menu);
void load_room_preferences(MenuShell& menu);
void shutdown_room_session(MenuShell& menu);
