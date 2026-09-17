#include "../src/net_session.hpp"

#include <algorithm>
#include <cstdio>
#include <memory>
#include <filesystem>
#include <vector>

namespace {

struct DelayedPacket {
    int due = 0;
    NetEndpoint to{};
    std::vector<std::uint8_t> bytes;
};

struct LossyRelay {
    UdpSocket socket;
    NetEndpoint host{};
    NetEndpoint client{};
    std::vector<DelayedPacket> queued;
    int seen = 0;
    std::uint32_t loss_rng = 0xB419C331U;

    void pump(int tick) {
        std::string error;
        for (int received = 0; received < 256; ++received) {
            Datagram packet;
            if (!socket.poll(packet, error)) break;
            const bool from_host = packet.from == host;
            if (!from_host) client = packet.from;
            ++seen;
            loss_rng ^= loss_rng << 13;
            loss_rng ^= loss_rng >> 17;
            loss_rng ^= loss_rng << 5;
            if (loss_rng % 10 == 0) continue;
            queued.push_back({tick + 6 + seen % 9, from_host ? client : host,
                              std::move(packet.bytes)});
        }
        for (std::size_t index = 0; index < queued.size();) {
            if (queued[index].due > tick) { ++index; continue; }
            if (!socket.send(queued[index].to, queued[index].bytes, error)) {
                std::fprintf(stderr, "relay send failed: %s\n", error.c_str());
            }
            queued.erase(queued.begin() + static_cast<std::ptrdiff_t>(index));
        }
    }
};

const RollbackFrame* frame_at(const RollbackSession& session, std::uint64_t tick) {
    for (const RollbackFrame& frame : session.frames)
        if (frame.tick == tick) return &frame;
    return nullptr;
}

} // namespace

int main() {
    auto host = std::make_unique<NetSession>();
    std::array<std::unique_ptr<NetSession>, 3> clients;
    std::array<LossyRelay, 3> relays;
    const auto log_root = std::filesystem::temp_directory_path() /
        ("gauche-loss-regression-" + std::to_string(network_clock_ms()));
    std::string error;
    if (!host_game(*host, 0, 82413, DeathPolicy::NextFloor, error)) return 1;
    for (int index = 0; index < 3; ++index) {
        clients[index] = std::make_unique<NetSession>();
        clients[index]->diagnostics.directory = (log_root / ("client-" + std::to_string(index))).string();
        auto& relay = relays[index];
        relay.loss_rng += static_cast<std::uint32_t>(index * 17);
        if (!relay.socket.open(0, error) ||
            !resolve_endpoint("127.0.0.1", host->socket.bound_port(), relay.host, error) ||
            !join_game(*clients[index], "127.0.0.1", relay.socket.bound_port(),
                       static_cast<std::uint64_t>(0xA3419 + index), error)) {
            std::fprintf(stderr, "lossy setup failed: %s\n", error.c_str()); return 1;
        }
    }
    // Delayed, reordered and 10% dropped UDP packets, with two clients polling at 30 Hz.
    bool started = false;
    int playing = 0;
    for (int tick = 1; tick <= 2200; ++tick) {
        const auto now = static_cast<std::uint64_t>(tick) * 17;
        for (int index = 0; index < 3; ++index) {
            if (index == 0 || tick % 2 == 0) pump_network(*clients[index], now);
            relays[index].pump(tick);
        }
        pump_network(*host, now);
        for (int index = 0; index < 3; ++index) {
            relays[index].pump(tick);
            if (index == 0 || tick % 2 == 0) pump_network(*clients[index], now);
        }
        if (!started) {
            started = std::all_of(clients.begin(), clients.end(), [](const auto& c) { return c->ready; }) &&
                std::all_of(host->peers.begin(), host->peers.end(), [](const auto& p) { return p.second.connected && p.second.snapshot.id == 0; });
        }
        if (started && playing == 600) {
            for (auto& client : clients) for (int step = 0; step < 8 && client->ready &&
                 !client->rollback.needs_snapshot && client->rollback.game.tick < host->rollback.game.tick; ++step)
                predict_frame(client->rollback, {});
        }
        if (started && playing < 600) {
            ++playing;
            step_network_game(*host, {});
            for (int index = 0; index < 3; ++index) {
                auto& client = *clients[index];
                if (index != 0 && tick % 2 != 0) continue;
                // One lagging client deliberately exercises corrections beyond its simulated tick.
                if (index == 2) {
                    for (int count = 0; count < 8 && client.rollback.game.tick + 2 < client.host_tick; ++count)
                        step_network_game(client, {});
                } else catch_up_network_client(client);
                Input remote;
                remote.move = (playing + index*7) % 24 < 12 ? Cell{1, 0} : Cell{-1, 0};
                for (int step = 0; step < (index == 0 ? 1 : 2); ++step) step_network_game(client, remote);
            }
        }
    }
    bool okay = playing == 600 && host->rollback.rollback_count > 0;
    for (int index = 0; index < 3; ++index) {
        const auto& client = *clients[index];
        const auto common = client.rollback.confirmed_through;
        const auto* authoritative = frame_at(host->rollback, common);
        const auto* predicted = frame_at(client.rollback, common);
        const auto hash = predicted ? predicted->hash_after : game_hash(client.rollback.game);
        const bool matches = authoritative && authoritative->hash_after == hash;
        std::printf("client=%d confirmed=%llu host=%llu recoveries=%d rtt=%llu matching=%d\n", index+1,
            static_cast<unsigned long long>(common), static_cast<unsigned long long>(host->rollback.game.tick),
            client.diagnostics.recovery_count, static_cast<unsigned long long>(client.round_trip_ms), matches);
        std::printf("local tick=%llu revision=%u/%u pending=%zu reason=%s log=%s\n",
            static_cast<unsigned long long>(client.rollback.game.tick), client.timeline_revision, host->timeline_revision,
            client.rollback.pending.size(), client.rollback.recovery_reason, client.diagnostics.log_path.c_str());
        okay &= client.ready && !client.rollback.needs_snapshot && common >= 570 && matches &&
                 client.diagnostics.recovery_count == 0;
    }
    if (!okay) { std::fprintf(stderr, "four-player lossy session failed\n"); return 1; }
    std::filesystem::remove_all(log_root);
    std::puts("four-player lossy session passed");
    return 0;
}
