#include "party.hpp"
#include "../net_session_internal.hpp"

void send_party_state(NetSession& session) {
    if (session.role == NetRole::Host) {
        std::uint8_t ready = session.party_ready ? 1 : 0;
        for (int owner = 1; owner < 4; ++owner)
            if (session.peers[static_cast<std::size_t>(owner)].connected &&
                session.peers[static_cast<std::size_t>(owner)].party_ready)
                ready |= static_cast<std::uint8_t>(1U << owner);
        session.party_ready_mask = ready;
        PacketWriter packet = begin_packet(WireKind::PartyState);
        packet.u8(session.match_started ? 1 : 0);
        packet.u8(ready);
        for (int owner = 1; owner < 4; ++owner) {
            const auto& peer = session.peers[static_cast<std::size_t>(owner)];
            if (peer.connected) send_wire(session, peer.endpoint, packet);
        }
    } else if (session.role == NetRole::Client && session.ready) {
        PacketWriter packet = begin_packet(WireKind::PartyReady);
        packet.u64(session.local_identity);
        packet.u8(session.party_ready ? 1 : 0);
        send_wire(session, session.host_endpoint, packet);
    }
}

bool receive_party_state(NetSession& session, const Datagram& datagram,
                         PacketReader& reader, WireKind kind) {
    if (kind == WireKind::PartyState) {
        if (session.role != NetRole::Client || datagram.from != session.host_endpoint) return true;
        const auto started = reader.u8(), ready = reader.u8();
        if (!reader.finished() || started > 1 || ready > 15) return true;
        session.match_started = started != 0;
        session.party_ready_mask = ready;
        session.last_host_packet_ms = session.now_ms;
        return true;
    }
    if (kind != WireKind::PartyReady) return false;
    if (session.role != NetRole::Host) return true;
    const auto identity = reader.u64();
    const auto ready = reader.u8();
    if (!reader.finished() || ready > 1) return true;
    for (int owner = 1; owner < 4; ++owner) {
        auto& peer = session.peers[static_cast<std::size_t>(owner)];
        if (!peer.connected || peer.endpoint != datagram.from || peer.identity != identity) continue;
        peer.party_ready = ready != 0;
        peer.last_heard_ms = session.now_ms;
        return true;
    }
    return true;
}

bool start_network_party(NetSession& session) {
    if (session.role != NetRole::Host || !session.party_ready) return false;
    for (const auto& peer : session.peers)
        if (peer.connected && (!peer.party_ready || peer.snapshot.id != 0)) return false;
    session.match_started = true;
    network_event(session, "party_started");
    send_party_state(session);
    return true;
}
