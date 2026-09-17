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
    for (auto& [owner, peer_entry] : session.peers)
        if (session.peers.at(owner).identity == identity)
            return owner;
    return -1;
}

void welcome(NetSession& session, NetEndpoint endpoint, std::uint64_t identity, int owner) {
    PacketWriter packet = begin_packet(WireKind::Welcome);
    packet.u64(identity);
    packet.i32(owner);
    packet.u64(session.rollback.game.tick);
    packet.u8(static_cast<std::uint8_t>(session.rollback.game.run.death_policy));
    send_wire(session, endpoint, packet);
}

Cell join_cell(const Game& game, int owner) {
    if (const auto inside = encounter_join_cell(game)) return *inside;
    (void)owner;
    return player_spawn_cell(game, game.run.spawn);
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
    const auto retained = session.departed.find(identity);
    bool new_player = false;
    if (owner < 0) {
        if (session.peers.size()+1 >= static_cast<std::size_t>(session.admission_limit) ||
            session.next_player_id == INT32_MAX) {
            welcome(session, datagram.from, identity, -1); return;
        }
        owner = retained != session.departed.end() ? retained->second.id : session.next_player_id++;
        auto& joining=session.peers[owner];
        if (retained != session.departed.end()) {
            joining.departed_player=retained->second.body;
            joining.departed_state=retained->second.state;
            joining.departed_floor=retained->second.floor;
        } else new_player=true;
    }
    NetPeer& peer = session.peers.at(owner);
    peer.identity = identity;
    peer.endpoint = datagram.from;
    peer.connected = true;
    peer.last_heard_ms = session.now_ms;
    peer.pending_inputs.clear();
    peer.late_inputs.clear();
    bool changed = false;
    if (new_player || !has_player(session.rollback.game, owner) ||
        get_entity(session.rollback.game, player_state(session.rollback.game, owner).controlled) == nullptr) {
        Game& game = session.rollback.game;
        const Handle player = spawn_entity(game, EntityKind::Player, join_cell(game, owner));
        if (player.slot<0) {
            session.peers.erase(owner); welcome(session,datagram.from,identity,-1); return;
        }
        player_state(game, owner) = peer.departed_state;
        player_state(game, owner).controlled = player;
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
        player_state(game, owner).online = true;
        if (game.run.phase == RunPhase::Reward) player_state(game, owner).chosen = true;
        if (game.run.phase == RunPhase::Shop) player_state(game, owner).shop_ready = true;
        changed = true;
    } else if (!player_state(session.rollback.game, owner).online) {
        Game& game = session.rollback.game;
        player_state(game, owner).online = true;
        if (Entity* entity = get_entity(game, player_state(game, owner).controlled)) {
            if (entity_at(game, entity->cell, true) >= 0)
                entity->cell = join_cell(game, owner);
            entity->impassable = entity->health > 0;
        }
        changed = true;
    }
    session.departed.erase(identity);
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
    NetPeer& peer = session.peers.at(owner);
    if (!peer.connected || peer.endpoint != datagram.from) return;
    peer.last_heard_ms = session.now_ms;
    acknowledge_corrections(session, owner, acknowledged_revision);
    peer.confirmed_tick = std::min(confirmed_tick, session.rollback.game.tick);
    const std::uint64_t current = session.rollback.game.tick;
    std::sort(inputs.begin(), inputs.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    for (const auto& [tick, input] : inputs) {
        if (tick == 0 || tick > current + 120) continue;
        if (tick > current) {
            peer.pending_inputs[tick] = input;
            continue;
        }
        if (tick > session.rollback.input_commit_tick) peer.late_inputs[tick] = input;
    }
}

void receive_snapshot_ack(NetSession& session, const Datagram& datagram, PacketReader& reader) {
    const std::uint64_t identity = reader.u64();
    const std::uint32_t id = reader.u32();
    if (!reader.finished()) return;
    const int owner = peer_for(session, identity);
    if (owner < 0) return;
    NetPeer& peer = session.peers.at(owner);
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

// Coalesce redundant packets before replaying history. A snapshot catch-up burst
// must not replay the same hundred ticks once per received datagram.
void apply_pending_host_inputs(NetSession& session) {
    for (auto& [owner, peer] : session.peers) {
        if (peer.late_inputs.empty()) continue;
        const std::vector<std::pair<std::uint64_t, Input>> late(peer.late_inputs.begin(),peer.late_inputs.end());
        peer.late_inputs.clear();
        const auto corrected = revise_host_inputs(session.rollback, owner, late);
        if (!corrected.empty()) {
            const auto earliest = corrected.front().tick;
            ++session.timeline_revision;
            for (auto& [target, recipient_entry] : session.peers) {
                auto& recipient = session.peers.at(target);
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
}

void disconnect_peer(NetSession& session, int owner) {
    auto& peer = session.peers.at(owner);
    if (!peer.connected) return;
    peer.connected = false;
    peer.party_ready = false;
    peer.pending_inputs.clear();
    peer.late_inputs.clear();
    peer.snapshot = {};
    Game& game = session.rollback.game;
    const auto handle = player_state(game, owner).controlled;
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
    auto saved=player_state(game,owner);
    saved.controlled={}; saved.online=false;
    if (game.run.phase==RunPhase::Reward && !saved.chosen && saved.pending_count<12) {
        saved.pending_offers[static_cast<std::size_t>(saved.pending_count++)]=saved.offers;
        saved.chosen=true;
    }
    session.departed[peer.identity]={owner,peer.departed_player,saved,peer.departed_floor};
    game.players.erase(owner);
    session.peers.erase(owner);
    advance_run(game);
    publish_host_state(session);
    send_party_state(session);
}

void publish_host_state(NetSession& session) {
    const Game changed = session.rollback.game;
    begin_rollback(session.rollback, changed);
    ++session.timeline_revision;
    for (auto& [owner, peer_entry] : session.peers) {
        NetPeer& peer = session.peers.at(owner);
        peer.pending_inputs.clear();
        peer.late_inputs.clear();
        peer.correction_from = 0;
        peer.correction_ranges.clear();
        if (peer.connected) queue_snapshot(session, owner);
    }
}

void restart_host_run(NetSession& session, std::uint64_t seed) {
    if (session.role != NetRole::Host) return;
    network_event(session, "host_restart", 0, seed);
    for (auto& [identity, retained] : session.departed) {
        retained.body={}; retained.state={}; retained.floor=0;
    }
    const DeathPolicy policy = session.rollback.game.run.death_policy;
    Game fresh;
    start_run(fresh, seed);
    fresh.tick = session.rollback.game.tick;
    fresh.run.death_policy = policy;
    for (auto& [owner, peer_entry] : session.peers) {
        NetPeer& peer = session.peers.at(owner);
        peer.departed_player = {};
        if (peer.identity == 0 || !peer.connected) continue;
        const Handle handle = spawn_entity(fresh, EntityKind::Player, join_cell(fresh, owner));
        player_state(fresh, owner).controlled = handle;
        if (Entity* player = get_entity(fresh, handle)) {
            player->owner = owner;
            player->inventory = {};
            insert_item(player->inventory, make_item(ItemKind::Fist));
            insert_item(player->inventory, make_item(ItemKind::Bandage, 3));
            player->impassable = peer.connected;
        }
        player_state(fresh, owner).online = peer.connected;
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
            session.peers.at(owner).endpoint == datagram.from) {
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
        if (owner >= 0 && session.peers.at(owner).endpoint == datagram.from &&
            session.peers.at(owner).snapshot.id == 0)
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
            NetPeer& peer = session.peers.at(owner);
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
    PlayerInputs inputs{};
    for (const auto& [owner, participant] : session.rollback.game.players)
        if (participant.online) inputs[owner] = missing_remote_input(session.rollback.game, owner);
    inputs[0] = local_input;
    for (auto& [owner, peer_entry] : session.peers) {
        NetPeer& peer = session.peers.at(owner);
        if (!peer.connected) continue;
        const auto found = peer.pending_inputs.find(tick);
        if (found != peer.pending_inputs.end()) {
            inputs[owner] = found->second;
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
    const auto frame_bytes = 20 + inputs.size()*44;
    const auto budget_count = std::max<std::size_t>(1, (game_datagram_bytes-16)/frame_bytes);
    const std::size_t count = std::min({canonical_frames_per_packet, budget_count, session.rollback.frames.size()});
    packet.u8(static_cast<std::uint8_t>(count));
    for (std::size_t index = session.rollback.frames.size() - count;
         index < session.rollback.frames.size(); ++index) {
        const RollbackFrame& frame = session.rollback.frames[index];
        write_frame(packet, {frame.tick, frame.inputs, frame.hash_after});
    }
    for (auto& [owner, peer_entry] : session.peers) {
        const NetPeer& peer = session.peers.at(owner);
        if (peer.connected && peer.snapshot.id == 0)
            send_wire(session, peer.endpoint, packet);
    }
}
