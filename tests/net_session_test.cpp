#include "../src/net_session.hpp"

#include <array>
#include <cstdio>
#include <memory>
#include <gubsy/realnet/rendezvous.hpp>
#include <gubsy/realnet/relay.hpp>

namespace {

// CLOCK: Advance virtual time explicitly; checks do not sleep for retry deadlines.
void pump_for_check(NetSession& session) {
    static std::uint64_t now_ms = 0;
    now_ms += 17;
    pump_network(session, now_ms);
}


bool traversal_failure_reports() {
    auto client = std::make_unique<NetSession>();
    UdpSocket server;
    std::string error;
    if (!server.open(0,error) || !client->socket.open(0,error)) return false;
    auto& t = client->traversal;
    resolve_endpoint("127.0.0.1",server.bound_port(),t.punch_server,error);
    t.relay_server = t.punch_server;
    t.phase = TraversalPhase::Relay; t.room = "CHECKS"; t.attempt = "attempt";
    t.allocation = "allocation"; t.punch_secret = "test-punch-secret";
    t.relay_secret = "test-relay-secret"; t.deadline_ms = 100;
    client->role = NetRole::Client; client->local_owner = -1;
    realnet::RelayPacket ready;
    ready.kind = realnet::RelayPacketKind::Ready; ready.role = realnet::RelayRole::Joiner; ready.room_code = t.room;
    ready.join_attempt_id = t.attempt; ready.allocation_id = t.allocation;
    ready.ts_ms = realnet::unix_time_ms() + 3600000;
    realnet::sign_relay_packet(ready, t.relay_secret);
    const auto bytes = realnet::encode_relay_packet(ready);
    Datagram datagram{t.relay_server, {bytes.begin(), bytes.end()}};
    receive_traversal(*client, datagram);
    if (t.last_reject != "traversal_clock_mismatch" || t.clock_delta_ms < 3500000) return false;
    client->now_ms = 101;
    step_traversal(*client);
    if (t.phase != TraversalPhase::Failed || client->status.find("automatic date/time") == std::string::npos)
        return false;
    int reports = 0;
    while (server.poll(datagram,error)) {
        realnet::Packet report;
        if (!realnet::decode_packet(std::string(datagram.bytes.begin(),datagram.bytes.end()),report,error) ||
            report.kind != realnet::PacketKind::PunchResult) continue;
        if (!realnet::verify_packet(report,t.punch_secret) ||
            report.result.find("traversal_clock_mismatch") == std::string::npos ||
            report.result.find("test-punch-secret") != std::string::npos ||
            report.result.find("test-relay-secret") != std::string::npos) return false;
        ++reports;
    }
    const int sent = t.reports;
    for (int i=0;i<20;++i) { client->now_ms += 1000; step_traversal(*client); }
    if (reports < 1 || t.reports != sent) return false;
    // Probes can succeed while the game handshake stalls: still try the relay.
    t.phase = TraversalPhase::Connected; t.deadline_ms = client->now_ms;
    t.terminal_reported = false; t.force_relay = false;
    client->host_endpoint = t.punch_server;
    step_traversal(*client);
    return t.phase == TraversalPhase::Relay;
}

bool server_clock_handshake() {
    auto client = std::make_unique<NetSession>();
    auto& t = client->traversal;
    t.phase = TraversalPhase::Relay; t.room = "CLOCKS"; t.attempt = "attempt";
    t.allocation = "allocation"; t.relay_secret = "relay-secret";
    t.relay_server = {123, 8790};
    // Deliberately disagree with this machine's wall clock by an hour.
    const auto service_now = realnet::unix_time_ms() + 3600000;
    t.clock = {service_now, steady_milliseconds()};
    realnet::RelayPacket ready;
    ready.kind = realnet::RelayPacketKind::Ready; ready.role = realnet::RelayRole::Joiner;
    ready.room_code = t.room; ready.join_attempt_id = t.attempt;
    ready.allocation_id = t.allocation; ready.ts_ms = service_now;
    const auto deliver = [&] {
        realnet::sign_relay_packet(ready,t.relay_secret);
        const auto bytes = realnet::encode_relay_packet(ready);
        Datagram datagram{t.relay_server,{bytes.begin(),bytes.end()}};
        receive_traversal(*client,datagram);
    };
    deliver();
    if (t.phase != TraversalPhase::Connected || !client->host_endpoint.relayed || t.rejected != 0)
        return false;
    ready.ts_ms = service_now - 31000;
    deliver();
    if (t.last_reject != "traversal_clock_mismatch") return false;
    const auto now = steady_milliseconds();
    t.clock = {service_now,now-10000};
    const auto advanced = server_time_ms(t.clock);
    return advanced >= service_now+10000 && advanced < service_now+11000;
}

bool join_running_floor_and_respawn() {
    auto host = std::make_unique<NetSession>();
    auto client = std::make_unique<NetSession>();
    std::string error;
    if (!host_game(*host, 0, 27, DeathPolicy::NextFloor, error)) return false;
    Game& game = host->rollback.game;
    game.run.floor = 3;
    generate_world_floor(game);
    game.tick = 900;
    const Cell spawn = game.run.spawn;
    // The old join search accepted walkable lava and burning ground.
    if (auto* tile = game.stage.at(spawn + Cell{1, 0})) tile->kind = TileKind::Lava;
    if (!join_game(*client, "127.0.0.1", host->socket.bound_port(), 0x9876, error)) return false;
    for (int i = 0; i < 60 && !client->ready; ++i) {
        pump_for_check(*client); pump_for_check(*host); pump_for_check(*client);
    }
    const Entity* joined = get_entity(game, game.players[1]);
    if (!client->ready || !client->match_started || !joined || game.tick != 900 ||
        distance(joined->cell, spawn) > 9 ||
        game.stage.at_or_border(joined->cell).kind == TileKind::Lava ||
        game_hash(game) != game_hash(client->rollback.game)) return false;
    // Surviving host finishes the floor; the dead guest must not block rewards.
    get_entity(game, game.players[1])->health = 0;
    finish_floor(game);
    if (!game.run.chosen[1]) return false;
    game.run.chosen[0] = true;
    advance_run(game);
    const Entity* revived = get_entity(game, game.players[1]);
    if (game.run.floor != 4 || !revived || revived->health != revived->max_health) return false;
    return true;
}

bool four_players() {
    NetSession host;
    std::array<NetSession, 3> clients{};
    std::string error;
    if (!host_game(host, 0, 1717, DeathPolicy::Entrance, error)) return false;
    for (int index = 0; index < 3; ++index) {
        if (!join_game(clients[static_cast<std::size_t>(index)], "127.0.0.1",
                       host.socket.bound_port(), static_cast<std::uint64_t>(300 + index),
                       error)) return false;
        for (int iteration = 0; iteration < 120; ++iteration) {
            for (NetSession& client : clients)
                if (client.role == NetRole::Client) pump_for_check(client);
            pump_for_check(host);
            for (NetSession& client : clients)
                if (client.role == NetRole::Client) pump_for_check(client);
        }
    }
    int players = 0;
    for (const Entity& entity : host.rollback.game.entities)
        if (entity.kind == EntityKind::Player) ++players;
    if (players != 4) return false;
    for (int index = 0; index < 3; ++index) {
        const NetSession& client = clients[static_cast<std::size_t>(index)];
        if (!client.ready || client.local_owner != index + 1 ||
            game_hash(client.rollback.game) != game_hash(host.rollback.game)) return false;
    }
    return true;
}

} // namespace

int main() {
    if (!server_clock_handshake()) {
        std::fputs("server clock handshake failed\n", stderr); return 1;
    }
    if (!traversal_failure_reports()) {
        std::fputs("traversal diagnostics / fallback failed\n", stderr); return 1;
    }
    if (!join_running_floor_and_respawn()) {
        std::fputs("late join / next-floor revival failed\n", stderr); return 1;
    }
    NetSession host;
    NetSession client;
    std::string error;
    if (!host_game(host, 0, 91919, DeathPolicy::NextFloor, error) ||
        !join_game(client, "127.0.0.1", host.socket.bound_port(), 0x12345678, error)) {
        std::fprintf(stderr, "session setup failed: %s\n", error.c_str());
        return 1;
    }
    for (int iteration = 0; iteration < 120 && !client.ready; ++iteration) {
        pump_for_check(client);
        pump_for_check(host);
        pump_for_check(client);
    }
    if (!client.ready || client.local_owner != 1 ||
        get_entity(host.rollback.game, host.rollback.game.players[1]) == nullptr) {
        std::fprintf(stderr, "join failed: %s\n", client.status.c_str());
        return 1;
    }
    for (int tick = 0; tick < 80; ++tick) {
        pump_for_check(host);
        pump_for_check(client);
        Input host_input;
        Input client_input;
        if (tick % 12 < 6) host_input.move = {1, 0};
        if (tick % 10 < 5) client_input.move = {-1, 0};
        step_network_game(host, host_input);
        step_network_game(client, client_input);
    }
    for (int iteration = 0; iteration < 10; ++iteration) {
        pump_for_check(host);
        pump_for_check(client);
    }
    if (host.rollback.game.tick != client.rollback.game.tick ||
        game_hash(host.rollback.game) != game_hash(client.rollback.game)) {
        std::fprintf(stderr, "session diverged host=%016llx client=%016llx status=%s\n",
                     static_cast<unsigned long long>(game_hash(host.rollback.game)),
                     static_cast<unsigned long long>(game_hash(client.rollback.game)),
                     client.status.c_str());
        return 1;
    }
    for (int idle = 0; idle < 450; ++idle) {
        pump_for_check(host);
        pump_for_check(client);
    }
    if (!client.ready || !host.peers[1].connected) {
        std::fputs("idle session lost its heartbeat\n", stderr);
        return 1;
    }
    const std::uint64_t previous_tick = host.rollback.game.tick;
    restart_host_run(host, 71234);
    for (int iteration = 0; iteration < 120 &&
         client.rollback.game.run.seed != 71234; ++iteration) {
        pump_for_check(client);
        pump_for_check(host);
        pump_for_check(client);
    }
    if (host.rollback.game.tick != previous_tick ||
        client.rollback.game.run.seed != 71234 ||
        client.rollback.game.run.floor != 1 ||
        game_hash(host.rollback.game) != game_hash(client.rollback.game)) {
        std::fputs("host restart did not synchronize a fresh run\n", stderr);
        return 1;
    }
    client.socket.close();
    const Handle original_slot = host.rollback.game.players[1];
    for (int iteration = 0; iteration < 370; ++iteration) pump_for_check(host);
    if (host.rollback.game.run.online[1]) {
        std::fputs("disconnected player still blocked the run\n", stderr);
        return 1;
    }
    NetSession rejoined;
    if (!join_game(rejoined, "127.0.0.1", host.socket.bound_port(), 0x12345678, error)) {
        std::fprintf(stderr, "reconnect socket failed: %s\n", error.c_str());
        return 1;
    }
    for (int iteration = 0; iteration < 120 && !rejoined.ready; ++iteration) {
        pump_for_check(rejoined);
        pump_for_check(host);
        pump_for_check(rejoined);
    }
    int players = 0;
    for (const Entity& entity : host.rollback.game.entities)
        if (entity.kind == EntityKind::Player) ++players;
    if (!rejoined.ready || rejoined.local_owner != 1 || players != 2 ||
        !host.rollback.game.run.online[1] || host.rollback.game.players[1] != original_slot) {
        std::fprintf(stderr, "reconnect duplicated or lost player: %s\n", rejoined.status.c_str());
        return 1;
    }
    if (!four_players()) {
        std::fputs("four-player topology failed\n", stderr);
        return 1;
    }
    std::puts("direct session passed");
    return 0;
}
