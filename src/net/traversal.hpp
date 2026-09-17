#pragma once

#include "../net_socket.hpp"
#include "server_clock.hpp"

#include <cstdint>
#include <string>
#include <vector>

struct NetSession;

enum class TraversalPhase { Off, Punch, Relay, Connected, Failed };

struct TraversalRoute {
    std::string attempt, punch_secret, allocation;
    NetEndpoint direct{}, relayed{};
    bool authenticated = false;
    std::uint64_t expires_ms = 0;
};

struct Traversal {
    TraversalPhase phase = TraversalPhase::Off;
    bool host = false, force_relay = false, relay_ready = false;
    std::string room, host_secret, attempt, punch_secret, allocation, relay_secret;
    NetEndpoint punch_server{}, relay_server{};
    ServerClock clock;
    std::uint64_t sequence = 1, next_hello_ms = 0, next_probe_ms = 0;
    std::uint64_t next_relay_ms = 0, deadline_ms = 0;
    std::vector<TraversalRoute> routes;
    // Bounded, credential-free join reports also reach the existing roomd journal.
    std::string last_reject;
    std::uint64_t punch_received = 0, relay_received = 0, rejected = 0, clock_delta_ms = 0;
    std::uint64_t next_report_ms = 0;
    int reports = 0;
    bool terminal_reported = false;
};

void step_traversal(NetSession& session);
bool receive_traversal(NetSession& session, Datagram& datagram);
bool send_traversal(NetSession& session, NetEndpoint target,
                    const std::vector<std::uint8_t>& bytes, std::string& error);
bool authorized_route(const NetSession& session, NetEndpoint source);
const char* traversal_status(const NetSession& session);

void report_traversal_join(NetSession& session);
