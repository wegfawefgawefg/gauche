#include "multiplayer.hpp"
#include "../app/options.hpp"
#include "../net/rooms/runtime.hpp"
#include "../net/party.hpp"
#include "../net_session.hpp"
#include <chrono>
#include <csignal>
#include <cstdio>
#include <memory>
#include <thread>

namespace {
volatile std::sig_atomic_t stopped = 0;
void stop(int) { stopped = 1; }
}

int run_headless_client(int argc, char** argv) {
    std::signal(SIGINT, stop); std::signal(SIGTERM, stop);
    RoomRequest request;
    request.url = value_arg(argc, argv, "--room-service");
    if (request.url.empty()) request.url = "https://45.77.123.14";
    request.code = value_arg(argc, argv, "--join-room");
    request.name = value_arg(argc, argv, "--player-name");
    if (request.name.empty()) request.name = "Headless bot";
    const std::string follow{value_arg(argc, argv, "--follow-host")};
    if (request.code.empty() && follow.empty()) {
        std::fprintf(stderr, "--headless requires --join-room CODE or --follow-host NAME\n"); return 2;
    }
    const auto started = network_clock_ms();
    const auto seconds = std::max(1, number_arg(value_arg(argc, argv, "--seconds")).value_or(600));
    const auto deadline = started + static_cast<std::uint64_t>(seconds) * 1000;
    // DISCOVERY: Match an explicitly named host, never silently join arbitrary public games.
    while (request.code.empty() && !stopped && network_clock_ms() < deadline) {
        request.operation = RoomOperation::Browse;
        const auto result = perform_room_request(request);
        for (const auto& room : result.rooms) if (room.host_name == follow && room.current_players < room.max_players) {
            request.code = room.room_code; break;
        }
        if (request.code.empty()) std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    if (request.code.empty()) return 1;
    request.operation = RoomOperation::Attempt;
    const auto result = perform_room_request(request);
    if (!result.okay) { std::fprintf(stderr, "%s\n", result.error.c_str()); return 1; }
    request.code = result.room.room_code;
    const auto connecting_since = network_clock_ms();
    auto session = std::make_unique<NetSession>();
    session->diagnostics.directory = (user_data_root() / "netlogs").string();
    std::string error;
    if (!join_game(*session, "127.0.0.1", 1, load_or_create_identity((user_data_root()/"player_id").string()), error)) return 1;
    session->match_started = false;
    auto& transport = session->traversal;
    transport.phase = TraversalPhase::Punch; transport.room = request.code;
    transport.punch_server = result.punch; transport.relay_server = result.relay;
    transport.attempt = result.attempt.join_attempt_id;
    transport.punch_secret = result.attempt.punch_secret;
    transport.allocation = result.attempt.relay_allocation_id;
    transport.relay_secret = result.attempt.relay_secret;
    transport.force_relay = has_arg(argc, argv, "--force-relay");
    request.token = result.attempt.join_token;
    MultiplayerDebug bot;
    bot.random = static_cast<std::uint32_t>(std::max(1, number_arg(value_arg(argc, argv, "--bot-seed")).value_or(1)));
    auto heartbeat = network_clock_ms(), report = heartbeat;
    auto previous = heartbeat;
    double accumulated = 0;
    bool joined = false;
    std::future<RoomResult> pending_heartbeat;
    while (!stopped && network_clock_ms() < deadline) {
        pump_network(*session);
        if (!joined && session->ready) {
            request.operation = RoomOperation::Finalize;
            const auto finalized = perform_room_request(request);
            if (!finalized.okay) { std::fprintf(stderr, "%s\n", finalized.error.c_str()); break; }
            request.member = finalized.member; request.token.clear(); joined = true;
            session->party_ready = true; send_party_state(*session);
            std::printf("Joined room %s as player %d (%s)\n", request.code.c_str(), session->local_owner + 1, traversal_status(*session));
        }
        const auto now = network_clock_ms();
        if (pending_heartbeat.valid() && pending_heartbeat.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            const auto response = pending_heartbeat.get();
            if (!response.okay) std::fprintf(stderr, "Room heartbeat: %s\n", response.error.c_str());
        }
        if (joined && now >= heartbeat && !pending_heartbeat.valid()) {
            heartbeat = now + 2000; request.operation = RoomOperation::Heartbeat;
            pending_heartbeat = std::async(std::launch::async, [copy = request] { return perform_room_request(copy); });
        }
        if (!joined && now > connecting_since + 25000) break;
        accumulated += std::min(static_cast<double>(now - previous)/1000.0, .25); previous = now;
        for (int count = 0; accumulated >= 1.0/60.0 && count < 15; ++count) {
            if (session->ready && session->match_started && !session->rollback.game.game_over)
                step_network_game(*session, multiplayer_bot_input(bot, session->rollback.game, session->local_owner));
            accumulated -= 1.0/60.0;
        }
        if (now >= report) {
            report = now + 5000;
            std::printf("tick=%llu ready=%d owner=%d status=%s\n",
                static_cast<unsigned long long>(session->rollback.game.tick), session->ready,
                session->local_owner, session->status.c_str()); std::fflush(stdout);
        }
        if (transport.phase == TraversalPhase::Failed) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    if (pending_heartbeat.valid()) (void)pending_heartbeat.get();
    if (!request.member.empty()) { request.operation = RoomOperation::Leave; (void)perform_room_request(request); }
    leave_network_game(*session);
    return joined ? 0 : 1;
}
