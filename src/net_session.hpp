#pragma once

#include "net_protocol.hpp"
#include "net_socket.hpp"

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

enum class NetRole { Solo, Host, Client };

struct SnapshotSend {
    std::uint32_t id = 0;
    std::uint64_t tick = 0;
    std::uint64_t checksum = 0;
    std::vector<std::uint8_t> bytes;
    std::uint64_t last_sent_pump = 0;
};

struct NetPeer {
    std::uint64_t identity = 0;
    NetEndpoint endpoint{};
    bool connected = false;
    std::uint64_t last_heard_pump = 0;
    std::map<std::uint64_t, Input> pending_inputs;
    SnapshotSend snapshot{};
};

struct SnapshotReceive {
    std::uint32_t id = 0;
    std::uint64_t tick = 0;
    std::uint64_t checksum = 0;
    std::uint32_t total_size = 0;
    std::vector<std::vector<std::uint8_t>> chunks;
    std::size_t received = 0;
};

struct CorrectionReceive {
    std::uint32_t id = 0;
    std::vector<std::vector<CanonicalFrame>> chunks;
    std::size_t received = 0;
};

struct NetSession {
    NetRole role = NetRole::Solo;
    UdpSocket socket;
    RollbackSession rollback;
    std::array<NetPeer, 4> peers{};
    NetEndpoint host_endpoint{};
    std::uint64_t local_identity = 0;
    int local_owner = 0;
    std::uint64_t host_tick = 0;
    std::uint64_t pump_tick = 0;
    std::uint64_t last_host_packet = 0;
    std::uint32_t next_transfer_id = 1;
    std::uint32_t last_correction_id = 0;
    std::uint32_t last_snapshot_id = 0;
    SnapshotReceive receiving_snapshot{};
    CorrectionReceive receiving_correction{};
    std::map<std::uint64_t, Input> sent_inputs;
    bool ready = false;
    std::string status;
};

bool host_game(NetSession& session, std::uint16_t port, std::uint64_t seed,
               DeathPolicy policy, std::string& error);
bool join_game(NetSession& session, const std::string& host, std::uint16_t port,
               std::uint64_t identity, std::string& error);
void pump_network(NetSession& session);
void step_network_game(NetSession& session, Input local_input);
void restart_host_run(NetSession& session, std::uint64_t seed);
std::uint64_t load_or_create_identity(const std::string& path);
