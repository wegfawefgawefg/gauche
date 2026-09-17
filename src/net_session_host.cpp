#include "net_session_internal.hpp"
#include "net/party.hpp"
#include "world/encounter.hpp"
#include "items/action.hpp"
#include "items/sled.hpp"
#include "entities/river_raft.hpp"
#include "items/ice_anchor.hpp"
#include "items/pocket_door.hpp"

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
                if (tile != nullptr && walkable(*tile) && tile->kind != TileKind::Lava &&
                    tile->kind != TileKind::IceHole && tile->surface.fire_ticks == 0 &&
                    entity_at(game, candidate, true) < 0) return candidate;
            }
        }
    }
    return game.run.spawn;
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
    peer.last_heard_ms = session.now_ms;
    peer.pending_inputs.clear();
    bool changed = false;
    if (new_player || get_entity(session.rollback.game, session.rollback.game.players[static_cast<std::size_t>(owner)]) == nullptr) {
        Game& game = session.rollback.game;
        const Handle player = spawn_entity(game, EntityKind::Player, join_cell(game, owner));
        game.players[static_cast<std::size_t>(owner)] = player;
        if (Entity* entity = get_entity(game, player)) {
            const Entity& saved = peer.departed_player;
            if (saved.kind == EntityKind::Player) {
                const bool next_floor = peer.departed_floor != game.run.floor;
                const auto generation = entity->generation;
                const Cell cell = entity->cell;
                if (!next_floor) *entity = saved;
                else {
                    entity->inventory = saved.inventory;
                    entity->light = saved.light;
                    entity->self_light = saved.self_light;
                    entity->health = saved.health;
                    entity->max_health = saved.max_health;
                    entity->move_interval = saved.move_interval;
                    entity->artifacts = saved.artifacts;
                }
                entity->generation = generation;
                entity->cell = cell;
                if (next_floor) {
                    for (Item& item : entity->inventory.slots) {
                        fold_unused_door(item); fold_ice_anchor(item);
                    }
                    if (entity->health <= 0 && game.run.death_policy != DeathPolicy::NoRespawn) {
                        entity->health = entity->max_health;
                        entity->sprite = Sprite::Player;
                    }
                }
                entity->impassable = entity->health > 0;
            } else {
                entity->inventory = {};
                insert_item(entity->inventory, make_item(ItemKind::Fist));
                insert_item(entity->inventory, make_item(ItemKind::Bandage, 3));
            }
            entity->owner = owner;
            peer.departed_player = {};
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
    network_event(session, new_player ? "peer_join" : "peer_rejoin", owner);
    welcome(session, datagram.from, identity, owner);
    send_party_state(session);
    if (changed) publish_host_state(session);
    else if (peer.snapshot.id == 0) queue_snapshot(session, owner);
    else send_snapshot_chunks(session, owner);
}

void receive_input(NetSession& session, const Datagram& datagram, PacketReader& reader) {
    const std::uint64_t identity = reader.u64();
    const std::uint32_t acknowledged_revision = reader.u32();
    const std::uint64_t confirmed_tick = reader.u64();
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
    peer.last_heard_ms = session.now_ms;
    acknowledge_corrections(session, owner, acknowledged_revision);
    peer.confirmed_tick = std::min(confirmed_tick, session.rollback.game.tick);
    const std::uint64_t current = session.rollback.game.tick;
    std::sort(inputs.begin(), inputs.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    std::vector<std::pair<std::uint64_t, Input>> late;
    for (const auto& [tick, input] : inputs) {
        if (tick == 0 || tick > current + 120) continue;
        if (tick > current) {
            peer.pending_inputs[tick] = input;
            continue;
        }
        late.push_back({tick, input});
    }
    const auto corrected = revise_host_inputs(session.rollback, owner, late);
    if (!corrected.empty()) {
        const auto earliest = corrected.front().tick;
        ++session.timeline_revision;
        for (int target = 1; target < 4; ++target) {
            auto& recipient = session.peers[static_cast<std::size_t>(target)];
            if (!recipient.connected) continue;
            recipient.correction_ranges[session.timeline_revision] = earliest;
            // A peer unable to acknowledge for a whole history window needs a snapshot.
            if (recipient.correction_ranges.size() > 512) {
                recipient.correction_ranges.clear();
                recipient.correction_ranges[session.timeline_revision] = recipient.correction_from;
            }
            if (recipient.correction_from == 0) recipient.correction_from = earliest;
            else recipient.correction_from = std::min(recipient.correction_from, earliest);
        }
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
    peer.confirmed_tick = snapshot_tick;
    acknowledge_corrections(session, owner, peer.snapshot.revision);
    network_event(session, "snapshot_ack", owner, snapshot_tick);
    peer.snapshot = {};
    peer.last_heard_ms = session.now_ms;
    send_history_since(session, owner, snapshot_tick);
}

} // namespace

void disconnect_peer(NetSession& session, int owner) {
    auto& peer = session.peers[static_cast<std::size_t>(owner)];
    if (!peer.connected) return;
    peer.connected = false;
    peer.party_ready = false;
    peer.pending_inputs.clear();
    peer.snapshot = {};
    Game& game = session.rollback.game;
    const auto handle = game.players[static_cast<std::size_t>(owner)];
    if (Entity* player = get_entity(game, handle)) {
        cancel_item_action(*player);
        clear_sled_links(game,*player);
        clear_river_raft(game,*player);
        for (Item& item : player->inventory.slots) {
            sync_ice_anchor(game,item);
            if (item.flight.slot >= 0) item = {};
        }
        peer.departed_player = *player;
        peer.departed_player.toss = {};
        peer.departed_floor = game.run.floor;
        remove_entity(game, handle);
    }
    game.players[static_cast<std::size_t>(owner)] = {};
    game.run.online[static_cast<std::size_t>(owner)] = false;
    advance_run(game);
    publish_host_state(session);
    send_party_state(session);
}

void publish_host_state(NetSession& session) {
    const Game changed = session.rollback.game;
    begin_rollback(session.rollback, changed);
    ++session.timeline_revision;
    for (int owner = 1; owner < 4; ++owner) {
        NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
        peer.pending_inputs.clear();
        peer.correction_from = 0;
        peer.correction_ranges.clear();
        if (peer.connected) queue_snapshot(session, owner);
    }
}

void restart_host_run(NetSession& session, std::uint64_t seed) {
    if (session.role != NetRole::Host) return;
    network_event(session, "host_restart", 0, seed);
    const DeathPolicy policy = session.rollback.game.run.death_policy;
    Game fresh;
    start_run(fresh, seed);
    fresh.tick = session.rollback.game.tick;
    fresh.run.death_policy = policy;
    for (int owner = 1; owner < 4; ++owner) {
        NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
        peer.departed_player = {};
        if (peer.identity == 0 || !peer.connected) continue;
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
    case WireKind::Leave: {
        const auto identity = reader.u64();
        const int owner = peer_for(session, identity);
        if (reader.finished() && owner > 0 &&
            session.peers[static_cast<std::size_t>(owner)].endpoint == datagram.from) {
            network_event(session, "peer_leave", owner);
            disconnect_peer(session, owner);
        }
        break;
    }
    case WireKind::Input: receive_input(session, datagram, reader); break;
    case WireKind::SnapshotRequest: {
        const std::uint64_t identity = reader.u64();
        if (!reader.finished()) break;
        const int owner = peer_for(session, identity);
        if (owner >= 0 && session.peers[static_cast<std::size_t>(owner)].endpoint == datagram.from &&
            session.peers[static_cast<std::size_t>(owner)].snapshot.id == 0)
            queue_snapshot(session, owner);
        break;
    }
    case WireKind::SnapshotAck: receive_snapshot_ack(session, datagram, reader); break;
    case WireKind::Heartbeat: {
        const std::uint64_t identity = reader.u64();
        const std::uint32_t revision = reader.u32();
        const std::uint64_t confirmed_tick = reader.u64();
        const std::uint64_t echoed_ms = reader.u64();
        if (!reader.finished()) break;
        const int owner = peer_for(session, identity);
        if (owner >= 0) {
            NetPeer& peer = session.peers[static_cast<std::size_t>(owner)];
            if (peer.connected && peer.endpoint == datagram.from) {
                peer.last_heard_ms = session.now_ms;
                acknowledge_corrections(session, owner, revision);
                peer.confirmed_tick = std::min(confirmed_tick, session.rollback.game.tick);
                if (revision == session.timeline_revision) send_history_since(session, owner, peer.confirmed_tick);
                PacketWriter heartbeat = begin_packet(WireKind::Heartbeat);
                heartbeat.u64(echoed_ms);
                heartbeat.u64(session.rollback.game.tick);
                send_wire(session, peer.endpoint, heartbeat);
            }
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
    for (int owner = 0; owner < 4; ++owner)
        inputs[static_cast<std::size_t>(owner)] = missing_remote_input(session.rollback.game, owner);
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
    const auto previous_phase = session.rollback.game.run.phase;
    const auto previous_floor = session.rollback.game.run.floor;
    predict_frame(session.rollback, inputs);
    if (previous_phase != session.rollback.game.run.phase || previous_floor != session.rollback.game.run.floor)
        network_event(session, "run_transition");
    confirm_host_current(session.rollback);
    PacketWriter packet = begin_packet(WireKind::Canonical);
    packet.u32(session.timeline_revision);
    const std::size_t count = std::min(canonical_frames_per_packet, session.rollback.frames.size());
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
