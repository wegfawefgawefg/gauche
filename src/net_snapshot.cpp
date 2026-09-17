#include "net_session_internal.hpp"
#include "net/snapshot_codec.hpp"

#include <algorithm>

namespace {

constexpr std::size_t chunk_bytes = snapshot_chunk_bytes;

void acknowledge(NetSession& session, std::uint32_t id) {
    PacketWriter ack = begin_packet(WireKind::SnapshotAck);
    ack.u64(session.local_identity);
    ack.u32(id);
    send_wire(session, session.host_endpoint, ack);
}

} // namespace

void queue_snapshot(NetSession& session, int owner) {
    if (owner <= 0 || !session.peers.contains(owner)) return;
    NetPeer& peer = session.peers.at(owner);
    if (!peer.connected) return;
    auto bytes=encode_network_snapshot(session.rollback.game);
    if(bytes.empty()){session.status="World snapshot exceeds transfer limit";return;}
    // Shared snapshots commit their baseline: late inputs cannot rewrite an in-flight world.
    session.rollback.input_commit_tick = session.rollback.game.tick;
    peer.correction_from = 0;
    peer.correction_ranges.clear();
    peer.correction = {};
    peer.snapshot = {};
    peer.snapshot.bytes = std::move(bytes);
    peer.snapshot.tick = session.rollback.game.tick;
    peer.snapshot.revision = session.timeline_revision;
    peer.snapshot.checksum = bytes_hash(peer.snapshot.bytes);
    peer.snapshot.id = session.next_transfer_id++;
    network_event(session, "snapshot_queued", owner, peer.snapshot.bytes.size());
    send_snapshot_chunks(session, owner);
}

void send_snapshot_chunks(NetSession& session, int owner) {
    NetPeer& peer = session.peers.at(owner);
    SnapshotSend& transfer = peer.snapshot;
    if (!peer.connected || transfer.id == 0 || transfer.bytes.empty()) return;
    const auto count = static_cast<std::uint16_t>(
        (transfer.bytes.size() + chunk_bytes - 1) / chunk_bytes);
    // PACING: Large worlds must not overflow a receiver before it can poll.
    const std::size_t end = std::min(transfer.next_chunk + 32, static_cast<std::size_t>(count));
    for (std::size_t cursor = transfer.next_chunk; cursor < end; ++cursor) {
        const auto index = static_cast<std::uint16_t>(cursor);
        const std::size_t start = static_cast<std::size_t>(index) * chunk_bytes;
        const std::size_t size = std::min(chunk_bytes, transfer.bytes.size() - start);
        PacketWriter packet = begin_packet(WireKind::SnapshotChunk);
        packet.u32(transfer.id);
        packet.u32(transfer.revision);
        packet.u64(transfer.tick);
        packet.u32(static_cast<std::uint32_t>(transfer.bytes.size()));
        packet.u16(index);
        packet.u16(count);
        packet.u64(transfer.checksum);
        packet.u16(static_cast<std::uint16_t>(size));
        packet.bytes.insert(packet.bytes.end(), transfer.bytes.begin() +
                            static_cast<std::ptrdiff_t>(start),
                            transfer.bytes.begin() + static_cast<std::ptrdiff_t>(start + size));
        send_wire(session, peer.endpoint, packet);
    }
    transfer.next_chunk = end == count ? 0 : end;
    transfer.next_send_ms = session.now_ms + (end == count ? 500 : 16);
}

void receive_snapshot_chunk(NetSession& session, PacketReader& reader) {
    const std::uint32_t id = reader.u32();
    const std::uint32_t revision = reader.u32();
    const std::uint64_t tick = reader.u64();
    const std::uint32_t total_size = reader.u32();
    const std::uint16_t index = reader.u16();
    const std::uint16_t count = reader.u16();
    const std::uint64_t checksum = reader.u64();
    const std::uint16_t payload_size = reader.u16();
    if (!reader.okay || id == 0 || total_size == 0 || total_size > snapshot_wire_limit ||
        count == 0 || count > snapshot_chunk_limit || index >= count || payload_size > chunk_bytes ||
        reader.position + payload_size != reader.bytes.size()) return;
    if (id <= session.last_snapshot_id) {
        acknowledge(session, id);
        return;
    }
    SnapshotReceive& transfer = session.receiving_snapshot;
    if (id < transfer.id) return; // A delayed old fragment must not discard newer progress.
    if (transfer.id != id) {
        transfer = {};
        transfer.id = id;
        transfer.revision = revision;
        transfer.tick = tick;
        transfer.total_size = total_size;
        transfer.checksum = checksum;
        transfer.chunks.resize(count);
    }
    if (transfer.revision != revision || transfer.tick != tick || transfer.total_size != total_size ||
        transfer.checksum != checksum || transfer.chunks.size() != count) return;
    auto& chunk = transfer.chunks[index];
    if (chunk.empty()) {
        chunk.assign(reader.bytes.begin() + static_cast<std::ptrdiff_t>(reader.position),
                     reader.bytes.end());
        ++transfer.received;
    }
    if (transfer.received != count || session.local_owner < 0) return;
    std::vector<std::uint8_t> bytes;
    bytes.reserve(total_size);
    for (const auto& part : transfer.chunks)
        bytes.insert(bytes.end(), part.begin(), part.end());
    if (bytes.size() != total_size || bytes_hash(bytes) != checksum) {
        session.status = "Snapshot checksum failed; waiting for retry";
        transfer = {};
        return;
    }
    Game restored;
    std::string error,diagnostic_note;
    if (!decode_network_snapshot(bytes, restored, error,diagnostic_note)) {
        session.status = error;
        transfer = {};
        return;
    }
    capture_network_recovery(session);
    network_event(session, "snapshot_applied", session.local_owner, tick);
    apply_host_snapshot(session.rollback, restored);
    session.sent_inputs.clear();
    session.host_tick = std::max(session.host_tick, tick);
    session.last_snapshot_id = id;
    session.timeline_revision = revision;
    session.receiving_correction = {};
    session.ready = true;
    session.status = "Joined as player " + std::to_string(session.local_owner + 1);
    if(!diagnostic_note.empty())session.status+="; "+diagnostic_note;
    acknowledge(session, id);
    transfer = {};
}

void send_history_since(NetSession& session, int owner, std::uint64_t after_tick) {
    const NetPeer& peer = session.peers.at(owner);
    if (!peer.connected || peer.snapshot.id != 0) return;
    if (!session.rollback.frames.empty() &&
        session.rollback.frames.front().tick > after_tick + 1) {
        queue_snapshot(session, owner);
        return;
    }
    std::vector<CanonicalFrame> history;
    for (const RollbackFrame& frame : session.rollback.frames)
        if (frame.tick > after_tick)
            history.push_back({frame.tick, frame.inputs, frame.hash_after});
    for (std::size_t start = 0; start < history.size(); start += canonical_frames_per_packet) {
        PacketWriter packet = begin_packet(WireKind::Canonical);
        packet.u32(session.timeline_revision);
        const std::size_t count = std::min(canonical_frames_per_packet, history.size() - start);
        packet.u8(static_cast<std::uint8_t>(count));
        for (std::size_t offset = 0; offset < count; ++offset)
            write_frame(packet, history[start + offset]);
        send_wire(session, peer.endpoint, packet);
    }
}
