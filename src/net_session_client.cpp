#include "net_session_internal.hpp"

#include <algorithm>

namespace {

void receive_welcome(NetSession& session, PacketReader& reader) {
    const std::uint64_t identity = reader.u64();
    const PlayerId owner = reader.i32();
    const std::uint64_t tick = reader.u64();
    const std::uint8_t death_policy = reader.u8();
    if (!reader.finished() || identity != session.local_identity) return;
    if (owner < 0) {
        session.status = "Host is full or uses different game content";
        return;
    }
    if (owner == 0 || death_policy >
        static_cast<std::uint8_t>(DeathPolicy::NextFloor)) return;
    session.local_owner = owner;
    session.host_tick = std::max(session.host_tick, tick);
    if (!session.ready) session.status = "Receiving world snapshot";
}

void receive_canonical(NetSession& session, PacketReader& reader) {
    const std::uint32_t revision = reader.u32();
    const std::uint8_t count = reader.u8();
    if (count == 0 || count > canonical_frames_per_packet) return;
    std::vector<CanonicalFrame> frames;
    frames.reserve(count);
    for (int index = 0; index < count; ++index) frames.push_back(read_frame(reader));
    if (!reader.finished()) return;
    for (const CanonicalFrame& frame : frames) {
        session.host_tick = std::max(session.host_tick, frame.tick);
        if (session.ready && revision == session.timeline_revision) confirm_frame(session.rollback, frame);
    }
}

void receive_correction(NetSession& session, PacketReader& reader) {
    const std::uint32_t id = reader.u32();
    const std::uint32_t revision = reader.u32();
    const std::uint16_t index = reader.u16();
    const std::uint16_t chunks = reader.u16();
    const std::uint8_t count = reader.u8();
    if (id == 0 || id <= session.last_correction_id || id <= session.last_snapshot_id ||
        revision < session.timeline_revision || chunks == 0 || chunks > max_correction_chunks ||
        index >= chunks || count == 0 || count > canonical_frames_per_packet) return;
    std::vector<CanonicalFrame> frames;
    frames.reserve(count);
    for (int offset = 0; offset < count; ++offset) frames.push_back(read_frame(reader));
    if (!reader.finished()) return;
    CorrectionReceive& transfer = session.receiving_correction;
    if (id < transfer.id) return;
    if (transfer.id != id) {
        transfer = {};
        transfer.id = id;
        transfer.revision = revision;
        transfer.chunks.resize(chunks);
    }
    if (transfer.chunks.size() != chunks || transfer.revision != revision) return;
    if (transfer.chunks[index].empty()) {
        transfer.chunks[index] = std::move(frames);
        ++transfer.received;
    }
    if (transfer.received != chunks || !session.ready) return;
    std::vector<CanonicalFrame> corrected;
    for (const auto& part : transfer.chunks)
        corrected.insert(corrected.end(), part.begin(), part.end());
    for (std::size_t offset = 1; offset < corrected.size(); ++offset) {
        if (corrected[offset].tick != corrected[offset - 1].tick + 1) {
            session.rollback.needs_snapshot = true;
            session.rollback.recovery_reason = "invalid_correction_sequence";
            session.rollback.recovery_tick = corrected[offset].tick;
            return;
        }
    }
    session.host_tick = std::max(session.host_tick, corrected.back().tick);
    apply_correction_batch(session.rollback, corrected);
    if (!session.rollback.needs_snapshot) session.timeline_revision = revision;
    session.last_correction_id = id;
    transfer = {};
}

} // namespace

void client_receive(NetSession& session, const Datagram&,
                    PacketReader& reader, WireKind kind) {
    switch (kind) {
    case WireKind::Welcome: receive_welcome(session, reader); break;
    case WireKind::Canonical: receive_canonical(session, reader); break;
    case WireKind::Correction: receive_correction(session, reader); break;
    case WireKind::SnapshotChunk: receive_snapshot_chunk(session, reader); break;
    case WireKind::Heartbeat: {
        const auto sent_ms = reader.u64();
        const auto tick = reader.u64();
        if (!reader.finished()) break;
        session.host_tick = std::max(session.host_tick, tick);
        if (sent_ms != 0 && sent_ms <= session.now_ms) {
            const auto rtt = session.now_ms - sent_ms;
            session.round_trip_ms = session.round_trip_ms == 0 ? rtt :
                (session.round_trip_ms * 3 + rtt) / 4;
            // host_tick is already one journey old; input must survive the return journey too.
            session.prediction_lead_ticks = static_cast<int>(std::clamp<std::uint64_t>(
                (session.round_trip_ms + 16) / 17 + 2, 2, 30));
        }
        break;
    }
    default: break;
    }
}

void client_step(NetSession& session, Input local_input) {
    if (session.local_owner < 0 || session.rollback.needs_snapshot ||
        session.rollback.game.game_over) return;
    const std::uint64_t tick = session.rollback.game.tick + 1;
    PlayerInputs inputs{};
    for (const auto& [owner, participant] : session.rollback.game.players)
        if (participant.online) inputs[owner] = missing_remote_input(session.rollback.game, owner);
    inputs[session.local_owner] = local_input;
    predict_frame(session.rollback, inputs);
    session.sent_inputs[tick] = local_input;
    while (session.sent_inputs.size() > 16) session.sent_inputs.erase(session.sent_inputs.begin());
    PacketWriter packet = begin_packet(WireKind::Input);
    packet.u64(session.local_identity);
    packet.u32(session.timeline_revision);
    packet.u64(session.rollback.confirmed_through);
    const std::size_t count = std::min<std::size_t>(8, session.sent_inputs.size());
    packet.u8(static_cast<std::uint8_t>(count));
    auto it = session.sent_inputs.rbegin();
    for (std::size_t index = 0; index < count; ++index, ++it) {
        packet.u64(it->first);
        packet.input(it->second);
    }
    send_wire(session, session.host_endpoint, packet);
}
