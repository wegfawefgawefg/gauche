#include "net_session_internal.hpp"
#include <algorithm>

void acknowledge_corrections(NetSession& session, int owner, std::uint32_t revision) {
    if (revision > session.timeline_revision) return;
    auto& peer = session.peers[static_cast<std::size_t>(owner)];
    if (peer.correction.id != 0 && revision >= peer.correction.revision) {
        peer.correction = {};
        peer.next_correction_ms = 0;
    }
    peer.correction_ranges.erase(peer.correction_ranges.begin(), peer.correction_ranges.upper_bound(revision));
    peer.correction_from = 0;
    for (const auto& [version, tick] : peer.correction_ranges) {
        (void)version;
        peer.correction_from = peer.correction_from == 0 ? tick : std::min(peer.correction_from, tick);
    }
}

void send_pending_correction(NetSession& session, int owner) {
    auto& peer = session.peers[static_cast<std::size_t>(owner)];
    if (!peer.connected || peer.snapshot.id != 0 || peer.correction_from == 0 ||
        session.now_ms < peer.next_correction_ms) return;
    auto& transfer = peer.correction;
    if (transfer.id == 0) {
        const auto& history = session.rollback.frames;
        const auto first = std::min(peer.correction_from, peer.confirmed_tick + 1);
        if (history.empty() || first < history.front().tick) {
            queue_snapshot(session, owner);
            return;
        }
        for (const auto& frame : history) if (frame.tick >= first)
            transfer.frames.push_back({frame.tick, frame.inputs, frame.hash_after});
        if (transfer.frames.empty()) return;
        transfer.id = session.next_transfer_id++;
        transfer.revision = session.timeline_revision;
    }
    const auto& frames = transfer.frames;
    const auto chunks = (frames.size() + canonical_frames_per_packet - 1) / canonical_frames_per_packet;
    if (chunks>max_correction_chunks) { queue_snapshot(session,owner); return; }
    for (std::size_t index = 0; index < chunks; ++index) {
        PacketWriter packet = begin_packet(WireKind::Correction);
        packet.u32(transfer.id);
        packet.u32(transfer.revision);
        packet.u16(static_cast<std::uint16_t>(index));
        packet.u16(static_cast<std::uint16_t>(chunks));
        const auto start = index * canonical_frames_per_packet;
        const auto count = std::min(canonical_frames_per_packet, frames.size() - start);
        packet.u8(static_cast<std::uint8_t>(count));
        for (std::size_t offset = 0; offset < count; ++offset) write_frame(packet, frames[start + offset]);
        send_wire(session, peer.endpoint, packet);
    }
    peer.next_correction_ms = session.now_ms + 150;
}
