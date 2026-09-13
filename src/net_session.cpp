#include "net_session_internal.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <random>

void send_wire(NetSession& session, NetEndpoint to, const PacketWriter& packet) {
    std::string error;
    if (!session.socket.send(to, packet.bytes, error)) session.status = error;
}

bool host_game(NetSession& session, std::uint16_t port, std::uint64_t seed,
               DeathPolicy policy, std::string& error) {
    if (!session.socket.open(port, error)) return false;
    Game initial;
    start_run(initial, seed);
    initial.run.death_policy = policy;
    begin_rollback(session.rollback, initial);
    session.role = NetRole::Host;
    session.local_owner = 0;
    session.ready = true;
    session.status = "Hosting on port " + std::to_string(session.socket.bound_port());
    return true;
}

bool join_game(NetSession& session, const std::string& host, std::uint16_t port,
               std::uint64_t identity, std::string& error) {
    if (identity == 0) { error = "Player identity cannot be zero"; return false; }
    if (!resolve_endpoint(host, port, session.host_endpoint, error) ||
        !session.socket.open(0, error)) return false;
    session.local_identity = identity;
    session.local_owner = -1;
    session.role = NetRole::Client;
    session.status = "Connecting to " + endpoint_text(session.host_endpoint);
    return true;
}

void pump_network(NetSession& session) {
    if (session.role == NetRole::Solo) return;
    ++session.pump_tick;
    for (int count = 0; count < 256; ++count) {
        Datagram datagram;
        std::string error;
        if (!session.socket.poll(datagram, error)) {
            if (!error.empty()) session.status = error;
            break;
        }
        PacketReader reader{datagram.bytes};
        WireKind kind{};
        if (!read_packet_header(reader, kind)) continue;
        if (session.role == NetRole::Host)
            host_receive(session, datagram, reader, kind);
        else if (datagram.from == session.host_endpoint) {
            session.last_host_packet = session.pump_tick;
            client_receive(session, datagram, reader, kind);
        }
    }
    if (session.role == NetRole::Host) {
        for (int owner = 1; owner < 4; ++owner) {
            NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
            if (!peer.connected) continue;
            if (session.pump_tick - peer.last_heard_pump > 360) {
                peer.connected = false;
                peer.pending_inputs.clear();
                Game& game = session.rollback.game;
                if (game.run.online[static_cast<std::size_t>(owner)]) {
                    game.run.online[static_cast<std::size_t>(owner)] = false;
                    if (Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]))
                        player->impassable = false;
                    advance_run(game);
                    publish_host_state(session);
                }
                continue;
            }
            if (peer.snapshot.id != 0 &&
                session.pump_tick - peer.snapshot.last_sent_pump >= 30)
                send_snapshot_chunks(session, owner);
        }
    } else {
        if (session.ready && session.pump_tick - session.last_host_packet > 360) {
            session.ready = false;
            session.status = "Connection lost; rejoining the same player slot";
        }
        if (!session.ready && session.pump_tick % 30 == 1) {
            PacketWriter hello = begin_packet(WireKind::Hello);
            hello.u64(session.local_identity);
            hello.u64(gameplay_version);
            send_wire(session, session.host_endpoint, hello);
        }
        if (session.ready && session.rollback.needs_snapshot &&
            session.pump_tick % 30 == 1) {
            PacketWriter request = begin_packet(WireKind::SnapshotRequest);
            request.u64(session.local_identity);
            send_wire(session, session.host_endpoint, request);
            session.status = "Requesting a fresh game snapshot";
        }
    }
}

void step_network_game(NetSession& session, Input local_input) {
    if (!session.ready) return;
    if (session.role == NetRole::Host) host_step(session, local_input);
    if (session.role == NetRole::Client) client_step(session, local_input);
}

std::uint64_t load_or_create_identity(const std::string& path) {
    std::uint64_t identity = 0;
    {
        std::ifstream input{path};
        if (input >> std::hex >> identity && identity != 0) return identity;
    }
    std::random_device random;
    identity = (static_cast<std::uint64_t>(random()) << 32) | random();
    if (identity == 0) identity = 1;
    std::filesystem::create_directories(std::filesystem::path{path}.parent_path());
    std::ofstream output{path};
    output << std::hex << identity << '\n';
    return identity;
}
