#pragma once

#include "net_protocol.hpp"
#include "net_socket.hpp"
#include "net/diagnostics.hpp"
#include "net/traversal.hpp"

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

enum class NetRole { Solo, Host, Client };

struct SnapshotSend {
    std::uint32_t revision = 0;
    std::uint32_t id = 0;
    std::uint64_t tick = 0;
    std::uint64_t checksum = 0;
    std::vector<std::uint8_t> bytes;
    std::uint64_t next_send_ms = 0;
    std::size_t next_chunk = 0;
};

struct CorrectionSend {
    std::uint32_t id = 0, revision = 0;
    std::vector<CanonicalFrame> frames;
};

struct NetPeer {
    std::uint64_t identity = 0;
    NetEndpoint endpoint{};
    bool connected = false;
    Entity departed_player{};
    int departed_floor = 0;
    bool party_ready = false;
    std::uint64_t last_heard_ms = 0;
    std::map<std::uint64_t, Input> pending_inputs;
    SnapshotSend snapshot{};
    CorrectionSend correction{};
    std::uint64_t confirmed_tick = 0;
    std::uint64_t correction_from = 0;
    std::map<std::uint32_t, std::uint64_t> correction_ranges;
    std::uint64_t next_correction_ms = 0;
};

struct SnapshotReceive {
    std::uint32_t revision = 0;
    std::uint32_t id = 0;
    std::uint64_t tick = 0;
    std::uint64_t checksum = 0;
    std::uint32_t total_size = 0;
    std::vector<std::vector<std::uint8_t>> chunks;
    std::size_t received = 0;
};

struct CorrectionReceive {
    std::uint32_t revision = 0;
    std::uint32_t id = 0;
    std::vector<std::vector<CanonicalFrame>> chunks;
    std::size_t received = 0;
};

struct NetSession {
    NetDiagnostics diagnostics;
    Traversal traversal;
    NetRole role = NetRole::Solo;
    UdpSocket socket;
    RollbackSession rollback;
    std::array<NetPeer, 4> peers{};
    NetEndpoint host_endpoint{};
    std::uint64_t local_identity = 0;
    int local_owner = 0;
    std::uint64_t host_tick = 0;
    int prediction_lead_ticks = 2;
    std::uint64_t round_trip_ms = 0;
    std::uint64_t now_ms = 0;
    std::uint64_t started_ms = 0;
    std::uint64_t next_heartbeat_ms = 0;
    std::uint64_t next_hello_ms = 0;
    std::uint64_t next_snapshot_request_ms = 0;
    bool clock_started = false;
    std::uint64_t last_host_packet_ms = 0;
    std::uint32_t next_transfer_id = 1;
    std::uint32_t timeline_revision = 0;
    std::uint32_t last_correction_id = 0;
    std::uint32_t last_snapshot_id = 0;
    SnapshotReceive receiving_snapshot{};
    CorrectionReceive receiving_correction{};
    std::map<std::uint64_t, Input> sent_inputs;
    bool ready = false;
    bool match_started = true, party_ready = true;
    std::uint8_t party_ready_mask = 1;
    std::string status;
};

bool host_game(NetSession& session, std::uint16_t port, std::uint64_t seed,
               DeathPolicy policy, std::string& error);
bool join_game(NetSession& session, const std::string& host, std::uint16_t port,
               std::uint64_t identity, std::string& error);
std::uint64_t network_clock_ms();
void pump_network(NetSession& session, std::uint64_t now_ms = network_clock_ms());
void step_network_game(NetSession& session, Input local_input);
void catch_up_network_client(NetSession& session);
void restart_host_run(NetSession& session, std::uint64_t seed);
void leave_network_game(NetSession& session);
std::uint64_t load_or_create_identity(const std::string& path);

bool network_end_confirmed(const NetSession& session);
