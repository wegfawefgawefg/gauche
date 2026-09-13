#include "../src/net_session.hpp"

#include <cstdio>

int main() {
    NetSession host;
    NetSession client;
    std::string error;
    if (!host_game(host, 0, 91919, DeathPolicy::NextFloor, error) ||
        !join_game(client, "127.0.0.1", host.socket.bound_port(), 0x12345678, error)) {
        std::fprintf(stderr, "session setup failed: %s\n", error.c_str());
        return 1;
    }
    for (int iteration = 0; iteration < 20 && !client.ready; ++iteration) {
        pump_network(client);
        pump_network(host);
        pump_network(client);
    }
    if (!client.ready || client.local_owner != 1 ||
        get_entity(host.rollback.game, host.rollback.game.players[1]) == nullptr) {
        std::fprintf(stderr, "join failed: %s\n", client.status.c_str());
        return 1;
    }
    for (int tick = 0; tick < 80; ++tick) {
        pump_network(host);
        pump_network(client);
        Input host_input;
        Input client_input;
        if (tick % 12 < 6) host_input.move = {1, 0};
        if (tick % 10 < 5) client_input.move = {-1, 0};
        step_network_game(host, host_input);
        step_network_game(client, client_input);
    }
    for (int iteration = 0; iteration < 10; ++iteration) {
        pump_network(host);
        pump_network(client);
    }
    if (host.rollback.game.tick != client.rollback.game.tick ||
        game_hash(host.rollback.game) != game_hash(client.rollback.game)) {
        std::fprintf(stderr, "session diverged host=%016llx client=%016llx status=%s\n",
                     static_cast<unsigned long long>(game_hash(host.rollback.game)),
                     static_cast<unsigned long long>(game_hash(client.rollback.game)),
                     client.status.c_str());
        return 1;
    }
    client.socket.close();
    for (int iteration = 0; iteration < 370; ++iteration) pump_network(host);
    NetSession rejoined;
    if (!join_game(rejoined, "127.0.0.1", host.socket.bound_port(), 0x12345678, error)) {
        std::fprintf(stderr, "reconnect socket failed: %s\n", error.c_str());
        return 1;
    }
    for (int iteration = 0; iteration < 20 && !rejoined.ready; ++iteration) {
        pump_network(rejoined);
        pump_network(host);
        pump_network(rejoined);
    }
    int players = 0;
    for (const Entity& entity : host.rollback.game.entities)
        if (entity.kind == EntityKind::Player) ++players;
    if (!rejoined.ready || rejoined.local_owner != 1 || players != 2) {
        std::fprintf(stderr, "reconnect duplicated or lost player: %s\n", rejoined.status.c_str());
        return 1;
    }
    std::puts("direct session passed");
    return 0;
}
