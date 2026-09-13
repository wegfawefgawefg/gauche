#include "../src/net_session.hpp"

#include <algorithm>
#include <cstdio>
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
            queued.push_back({tick + seen % 9, from_host ? client : host,
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
    NetSession host;
    NetSession client;
    LossyRelay relay;
    std::string error;
    if (!host_game(host, 0, 82413, DeathPolicy::NextFloor, error) ||
        !relay.socket.open(0, error) ||
        !resolve_endpoint("127.0.0.1", host.socket.bound_port(), relay.host, error) ||
        !join_game(client, "127.0.0.1", relay.socket.bound_port(), 0xA3419, error)) {
        std::fprintf(stderr, "lossy setup failed: %s\n", error.c_str());
        return 1;
    }
    for (int tick = 1; tick <= 400; ++tick) {
        pump_network(client);
        relay.pump(tick);
        pump_network(host);
        relay.pump(tick);
        pump_network(client);
        relay.pump(tick);
        Input remote;
        remote.move = tick % 24 < 12 ? Cell{1, 0} : Cell{-1, 0};
        step_network_game(host, {});
        if (client.ready) {
            for (int catchup = 0; catchup < 8 &&
                 client.rollback.game.tick + 2 < client.host_tick; ++catchup)
                step_network_game(client, {});
            step_network_game(client, remote);
        }
        relay.pump(tick);
    }
    for (int tick = 401; tick <= 1000; ++tick) {
        pump_network(client);
        relay.pump(tick);
        pump_network(host);
        relay.pump(tick);
        pump_network(client);
        relay.pump(tick);
    }
    const auto common = client.rollback.confirmed_through;
    const RollbackFrame* host_frame = frame_at(host.rollback, common);
    const RollbackFrame* client_frame = frame_at(client.rollback, common);
    const bool final_matches = client.rollback.game.tick == host.rollback.game.tick &&
        game_hash(client.rollback.game) == game_hash(host.rollback.game);
    const bool frame_matches = host_frame != nullptr && client_frame != nullptr &&
        host_frame->hash_after == client_frame->hash_after;
    if (!client.ready || client.rollback.needs_snapshot || common < 300 ||
        host.rollback.rollback_count == 0 || (!final_matches && !frame_matches)) {
        std::fprintf(stderr,
            "lossy sync failed: ready=%d need=%d confirmed=%llu host rollbacks=%llu "
            "host tick=%llu client tick=%llu relay packets=%d frames=%d/%d "
            "hash=%016llx/%016llx status=%s\n",
            client.ready ? 1 : 0, client.rollback.needs_snapshot ? 1 : 0,
            static_cast<unsigned long long>(common),
            static_cast<unsigned long long>(host.rollback.rollback_count),
            static_cast<unsigned long long>(host.rollback.game.tick),
            static_cast<unsigned long long>(client.rollback.game.tick),
            relay.seen,
            host_frame != nullptr ? 1 : 0, client_frame != nullptr ? 1 : 0,
            static_cast<unsigned long long>(game_hash(host.rollback.game)),
            static_cast<unsigned long long>(game_hash(client.rollback.game)),
            client.status.c_str());
        return 1;
    }
    std::puts("lossy direct session passed");
    return 0;
}
