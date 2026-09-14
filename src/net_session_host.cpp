#include "net_session_internal.hpp"
#include "world/encounter.hpp"

#include <algorithm>
#include <utility>

namespace {

int peer_for(const NetSession& session, std::uint64_t identity) {
    for (int owner = 1; owner < 4; ++owner)
        if (session.peers[static_cast<std::size_t>(owner)].identity == identity)
            return owner;
    return -1;
}

void welcome(NetSession& session, NetEndpoint endpoint, std::uint64_t identity, int owner) {
    PacketWriter packet = begin_packet(WireKind::Welcome);
    packet.u64(identity);
    packet.u8(static_cast<std::uint8_t>(owner < 0 ? 255 : owner));
    packet.u64(session.rollback.game.tick);
    packet.u8(static_cast<std::uint8_t>(session.rollback.game.run.death_policy));
    send_wire(session, endpoint, packet);
}

Cell join_cell(const Game& game, int owner) {
    if (const auto inside = encounter_join_cell(game)) return *inside;
    const Cell center = game.run.spawn + Cell{owner, 0};
    for (int radius = 0; radius <= 4; ++radius) {
        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                const Cell candidate = center + Cell{dx, dy};
                const Tile* tile = game.stage.at(candidate);
                if (tile != nullptr && walkable(*tile) &&
                    entity_at(game, candidate, true) < 0) return candidate;
            }
        }
    }
    return game.run.spawn;
}

void send_correction(NetSession& session, const std::vector<CanonicalFrame>& frames) {
    if (frames.empty()) return;
    if (frames.size() > 64) {
        for (int owner = 1; owner < 4; ++owner) queue_snapshot(session, owner);
        return;
    }
    const std::uint32_t id = session.next_transfer_id++;
    const std::size_t chunks = (frames.size() + 7) / 8;
    for (int copy = 0; copy < 2; ++copy) {
        for (std::size_t index = 0; index < chunks; ++index) {
            PacketWriter packet = begin_packet(WireKind::Correction);
            packet.u32(id);
            packet.u16(static_cast<std::uint16_t>(index));
            packet.u16(static_cast<std::uint16_t>(chunks));
            const std::size_t start = index * 8;
            const std::size_t count = std::min<std::size_t>(8, frames.size() - start);
            packet.u8(static_cast<std::uint8_t>(count));
            for (std::size_t offset = 0; offset < count; ++offset)
                write_frame(packet, frames[start + offset]);
            for (int owner = 1; owner < 4; ++owner) {
                const NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
                if (peer.connected) send_wire(session, peer.endpoint, packet);
            }
        }
    }
}

void accept_hello(NetSession& session, const Datagram& datagram, PacketReader& reader) {
    const std::uint64_t identity = reader.u64();
    const std::uint64_t version = reader.u64();
    if (!reader.finished() || identity == 0) return;
    if (version != gameplay_version) {
        welcome(session, datagram.from, identity, -1);
        return;
    }
    int owner = peer_for(session, identity);
    bool new_player = false;
    if (owner < 0) {
        for (int candidate = 1; candidate < 4; ++candidate) {
            if (session.peers[static_cast<std::size_t>(candidate)].identity == 0) {
                owner = candidate;
                new_player = true;
                break;
            }
        }
    }
    if (owner < 0) { welcome(session, datagram.from, identity, -1); return; }
    NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
    peer.identity = identity;
    peer.endpoint = datagram.from;
    peer.connected = true;
    peer.last_heard_pump = session.pump_tick;
    peer.pending_inputs.clear();
    bool changed = false;
    if (new_player) {
        Game& game = session.rollback.game;
        const Handle player = spawn_entity(game, EntityKind::Player, join_cell(game, owner));
        game.players[static_cast<std::size_t>(owner)] = player;
        if (Entity* entity = get_entity(game, player)) {
            entity->owner = owner;
            entity->inventory = {};
            insert_item(entity->inventory, make_item(ItemKind::Fist));
            insert_item(entity->inventory, make_item(ItemKind::Bandage, 3));
        }
        game.run.online[static_cast<std::size_t>(owner)] = true;
        if (game.run.phase == RunPhase::Reward) game.run.chosen[static_cast<std::size_t>(owner)] = true;
        if (game.run.phase == RunPhase::Shop) game.run.shop_ready[static_cast<std::size_t>(owner)] = true;
        changed = true;
    } else if (!session.rollback.game.run.online[static_cast<std::size_t>(owner)]) {
        Game& game = session.rollback.game;
        game.run.online[static_cast<std::size_t>(owner)] = true;
        if (Entity* entity = get_entity(game, game.players[static_cast<std::size_t>(owner)])) {
            if (entity_at(game, entity->cell, true) >= 0)
                entity->cell = join_cell(game, owner);
            entity->impassable = entity->health > 0;
        }
        changed = true;
    }
    welcome(session, datagram.from, identity, owner);
    if (changed) publish_host_state(session);
    else if (peer.snapshot.id == 0) queue_snapshot(session, owner);
    else send_snapshot_chunks(session, owner);
}

void receive_input(NetSession& session, const Datagram& datagram, PacketReader& reader) {
    const std::uint64_t identity = reader.u64();
    const std::uint8_t count = reader.u8();
    if (count == 0 || count > 8) return;
    std::vector<std::pair<std::uint64_t, Input>> inputs;
    inputs.reserve(count);
    for (int index = 0; index < count; ++index)
        inputs.push_back({reader.u64(), reader.input()});
    if (!reader.finished()) return;
    const int owner = peer_for(session, identity);
    if (owner < 0) return;
    NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
    if (!peer.connected || peer.endpoint != datagram.from) return;
    peer.last_heard_pump = session.pump_tick;
    const std::uint64_t current = session.rollback.game.tick;
    std::sort(inputs.begin(), inputs.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    std::uint64_t earliest = current + 1;
    for (const auto& [tick, input] : inputs) {
        if (tick == 0 || tick > current + 120) continue;
        if (tick > current) {
            peer.pending_inputs[tick] = input;
            continue;
        }
        const auto corrected = revise_host_input(session.rollback, tick, owner, input);
        if (!corrected.empty()) earliest = std::min(earliest, tick);
    }
    if (earliest <= current) {
        std::vector<CanonicalFrame> corrected;
        for (const RollbackFrame& frame : session.rollback.frames)
            if (frame.tick >= earliest)
                corrected.push_back({frame.tick, frame.inputs, frame.hash_after});
        send_correction(session, corrected);
    }
}

void receive_snapshot_ack(NetSession& session, const Datagram& datagram, PacketReader& reader) {
    const std::uint64_t identity = reader.u64();
    const std::uint32_t id = reader.u32();
    if (!reader.finished()) return;
    const int owner = peer_for(session, identity);
    if (owner < 0) return;
    NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
    if (peer.endpoint != datagram.from || peer.snapshot.id != id) return;
    const std::uint64_t snapshot_tick = peer.snapshot.tick;
    peer.snapshot = {};
    peer.last_heard_pump = session.pump_tick;
    send_history_since(session, owner, snapshot_tick);
}

} // namespace

void publish_host_state(NetSession& session) {
    const Game changed = session.rollback.game;
    begin_rollback(session.rollback, changed);
    for (int owner = 1; owner < 4; ++owner) {
        NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
        peer.pending_inputs.clear();
        if (peer.connected) queue_snapshot(session, owner);
    }
}

void restart_host_run(NetSession& session, std::uint64_t seed) {
    if (session.role != NetRole::Host) return;
    const DeathPolicy policy = session.rollback.game.run.death_policy;
    Game fresh;
    start_run(fresh, seed);
    fresh.tick = session.rollback.game.tick;
    fresh.run.death_policy = policy;
    for (int owner = 1; owner < 4; ++owner) {
        const NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
        if (peer.identity == 0) continue;
        const Handle handle = spawn_entity(fresh, EntityKind::Player, join_cell(fresh, owner));
        fresh.players[static_cast<std::size_t>(owner)] = handle;
        if (Entity* player = get_entity(fresh, handle)) {
            player->owner = owner;
            player->inventory = {};
            insert_item(player->inventory, make_item(ItemKind::Fist));
            insert_item(player->inventory, make_item(ItemKind::Bandage, 3));
            player->impassable = peer.connected;
        }
        fresh.run.online[static_cast<std::size_t>(owner)] = peer.connected;
    }
    session.rollback.game = std::move(fresh);
    publish_host_state(session);
}

void host_receive(NetSession& session, const Datagram& datagram,
                  PacketReader& reader, WireKind kind) {
    switch (kind) {
    case WireKind::Hello: accept_hello(session, datagram, reader); break;
    case WireKind::Input: receive_input(session, datagram, reader); break;
    case WireKind::SnapshotRequest: {
        const std::uint64_t identity = reader.u64();
        if (!reader.finished()) break;
        const int owner = peer_for(session, identity);
        if (owner >= 0 && session.peers[static_cast<std::size_t>(owner)].endpoint == datagram.from)
            queue_snapshot(session, owner);
        break;
    }
    case WireKind::SnapshotAck: receive_snapshot_ack(session, datagram, reader); break;
    case WireKind::Heartbeat: {
        const std::uint64_t identity = reader.u64();
        if (!reader.finished()) break;
        const int owner = peer_for(session, identity);
        if (owner >= 0) {
            NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
            if (peer.connected && peer.endpoint == datagram.from)
                peer.last_heard_pump = session.pump_tick;
        }
        break;
    }
    default: break;
    }
}

void host_step(NetSession& session, Input local_input) {
    if (session.rollback.game.game_over) return;
    const std::uint64_t tick = session.rollback.game.tick + 1;
    std::array<Input, 4> inputs{};
    inputs[0] = local_input;
    for (int owner = 1; owner < 4; ++owner) {
        NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
        if (!peer.connected) continue;
        const auto found = peer.pending_inputs.find(tick);
        if (found != peer.pending_inputs.end()) {
            inputs[static_cast<std::size_t>(owner)] = found->second;
            peer.pending_inputs.erase(found);
        }
    }
    predict_frame(session.rollback, inputs);
    confirm_host_current(session.rollback);
    PacketWriter packet = begin_packet(WireKind::Canonical);
    const std::size_t count = std::min<std::size_t>(6, session.rollback.frames.size());
    packet.u8(static_cast<std::uint8_t>(count));
    for (std::size_t index = session.rollback.frames.size() - count;
         index < session.rollback.frames.size(); ++index) {
        const RollbackFrame& frame = session.rollback.frames[index];
        write_frame(packet, {frame.tick, frame.inputs, frame.hash_after});
    }
    for (int owner = 1; owner < 4; ++owner) {
        const NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
        if (peer.connected && peer.snapshot.id == 0)
            send_wire(session, peer.endpoint, packet);
    }
}
