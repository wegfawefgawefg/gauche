#include "debug/performance.hpp"
#include "net_session_internal.hpp"
#include "net/party.hpp"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <random>

void send_wire(NetSession& session, NetEndpoint to, const PacketWriter& packet) {
    std::string error;
    if (!send_traversal(session, to, packet.bytes, error)) {
        session.status = error;
        network_event(session, "send_failed");
    } else {
        ++session.diagnostics.sent_packets;
        session.diagnostics.sent_bytes += packet.bytes.size();
    }
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
    begin_network_log(session);
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
    begin_network_log(session);
    session.status = "Connecting to " + endpoint_text(session.host_endpoint);
    return true;
}

std::uint64_t network_clock_ms() {
    using Clock = std::chrono::steady_clock;
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        Clock::now().time_since_epoch()).count());
}

void pump_network(NetSession& session, std::uint64_t now_ms) {
    PerfScope perf_scope(PerfZone::NetworkPump);
    if (session.role == NetRole::Solo) return;
    // CLOCK: Polling more often must not shorten reconnect or retry deadlines.
    session.now_ms = std::max(session.now_ms, now_ms);
    if (!session.clock_started) {
        session.clock_started = true;
        session.started_ms = session.now_ms;
    }
    step_traversal(session);
    const bool heartbeat_due = session.now_ms >= session.next_heartbeat_ms;
    if (heartbeat_due) session.next_heartbeat_ms = session.now_ms + 500;
    for (int count = 0; count < 256; ++count) {
        Datagram datagram;
        std::string error;
        if (!session.socket.poll(datagram, error)) {
            if (!error.empty()) session.status = error;
            break;
        }
        ++session.diagnostics.received_packets;
        session.diagnostics.received_bytes += datagram.bytes.size();
        if (receive_traversal(session, datagram)) continue;
        if (!authorized_route(session, datagram.from)) continue;
        PacketReader reader{datagram.bytes};
        WireKind kind{};
        if (!read_packet_header(reader, kind)) continue;
        if (receive_party_state(session, datagram, reader, kind)) continue;
        if (session.role == NetRole::Host)
            host_receive(session, datagram, reader, kind);
        else if (datagram.from == session.host_endpoint) {
            session.last_host_packet_ms = session.now_ms;
            client_receive(session, datagram, reader, kind);
            capture_network_recovery(session);
        }
    }
    report_traversal_join(session);
    if (session.now_ms >= session.diagnostics.next_report_ms) {
        session.diagnostics.next_report_ms = session.now_ms + 5000;
        network_event(session, "network_progress", session.local_owner, session.rollback.rollback_count);
    }
    if (heartbeat_due) send_party_state(session);
    if (session.role == NetRole::Host) {
        for (int owner = 1; owner < 4; ++owner) {
            NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
            if (!peer.connected) continue;
            if (session.now_ms - peer.last_heard_ms > 6000) {
                network_event(session, "peer_timeout", owner);
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
                session.now_ms >= peer.snapshot.next_send_ms)
                send_snapshot_chunks(session, owner);
            send_pending_correction(session, owner);
            if (heartbeat_due) {
                PacketWriter heartbeat = begin_packet(WireKind::Heartbeat);
                heartbeat.u64(0);
                heartbeat.u64(session.rollback.game.tick);
                send_wire(session, peer.endpoint, heartbeat);
            }
        }
    } else {
        if (session.ready && session.now_ms - session.last_host_packet_ms > 6000) {
            network_event(session, "host_timeout");
            session.ready = false;
            session.status = "Connection lost; rejoining the same player slot";
        }
        if (!session.ready && session.now_ms >= session.next_hello_ms &&
            (session.traversal.phase == TraversalPhase::Off ||
             session.traversal.phase == TraversalPhase::Connected)) {
            session.next_hello_ms = session.now_ms + 500;
            PacketWriter hello = begin_packet(WireKind::Hello);
            hello.u64(session.local_identity);
            hello.u64(gameplay_version);
            send_wire(session, session.host_endpoint, hello);
        }
        if (session.ready && heartbeat_due) {
            PacketWriter heartbeat = begin_packet(WireKind::Heartbeat);
            heartbeat.u64(session.local_identity);
            heartbeat.u32(session.timeline_revision);
            heartbeat.u64(session.rollback.confirmed_through);
            heartbeat.u64(session.now_ms);
            send_wire(session, session.host_endpoint, heartbeat);
        }
        if (session.ready && session.rollback.needs_snapshot &&
            session.now_ms >= session.next_snapshot_request_ms) {
            session.next_snapshot_request_ms = session.now_ms + 500;
            PacketWriter request = begin_packet(WireKind::SnapshotRequest);
            request.u64(session.local_identity);
            send_wire(session, session.host_endpoint, request);
            session.status = "Requesting a fresh game snapshot";
        }
    }
}

void catch_up_network_client(NetSession& session) {
    PerfScope perf_scope(PerfZone::Catchup);
    if (session.role != NetRole::Client || !session.ready || !session.match_started ||
        session.rollback.needs_snapshot) return;
    const auto target = session.host_tick + static_cast<std::uint64_t>(session.prediction_lead_ticks);
    for (int count = 0; count < 8 && !session.rollback.game.game_over &&
         session.rollback.game.tick < target; ++count)
        client_step(session, missing_remote_input(session.rollback.game, session.local_owner));
}

void step_network_game(NetSession& session, Input local_input) {
    PerfScope perf_scope(PerfZone::NetworkStep);
    if (!session.ready || !session.match_started) return;
    if (session.role == NetRole::Host) host_step(session, local_input);
    if (session.role == NetRole::Client) client_step(session, local_input);
}

void leave_network_game(NetSession& session) {
    network_event(session, "session_leave", session.local_owner);
    session.socket.close();
    session.traversal = {};
    session.role = NetRole::Solo;
    session.rollback = {};
    session.peers = {};
    session.host_endpoint = {};
    session.local_identity = 0;
    session.local_owner = 0;
    session.host_tick = 0;
    session.prediction_lead_ticks = 2;
    session.round_trip_ms = 0;
    session.now_ms = session.started_ms = 0;
    session.next_heartbeat_ms = session.next_hello_ms = session.next_snapshot_request_ms = 0;
    session.clock_started = false;
    session.last_host_packet_ms = 0;
    session.next_transfer_id = 1;
    session.timeline_revision = 0;
    session.last_correction_id = 0;
    session.last_snapshot_id = 0;
    session.receiving_snapshot = {};
    session.receiving_correction = {};
    session.sent_inputs.clear();
    session.ready = false;
    session.match_started = session.party_ready = true;
    session.party_ready_mask = 1;
    session.status.clear();
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
