#include "party.hpp"
#include "../net_session_internal.hpp"

void send_party_state(NetSession& session) {
    if (session.role == NetRole::Host) {
        session.ready_players.clear();
        if (session.party_ready) session.ready_players.push_back(0);
        for (const auto& [id, peer] : session.peers)
            if (peer.connected && peer.party_ready) session.ready_players.push_back(id);
        PacketWriter packet = begin_packet(WireKind::PartyState);
        packet.u8(session.match_started ? 1 : 0);
        packet.u32(static_cast<std::uint32_t>(session.ready_players.size()));
        for (PlayerId id : session.ready_players) packet.i32(id);
        for (const auto& [id, peer] : session.peers)
            if (peer.connected) send_wire(session, peer.endpoint, packet);
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
        const auto started = reader.u8();
        const auto count = reader.u32();
        if (!reader.okay || started>1 || count>(reader.bytes.size()-reader.position)/4) return true;
        std::vector<PlayerId> ready;
        PlayerId previous=-1;
        for (std::uint32_t index=0; index<count; ++index) {
            const auto id=reader.i32();
            if (id<0 || id<=previous) return true;
            ready.push_back(id); previous=id;
        }
        if (!reader.finished()) return true;
        session.match_started = started != 0;
        session.ready_players = std::move(ready);
        session.last_host_packet_ms = session.now_ms;
        return true;
    }
    if (kind != WireKind::PartyReady) return false;
    if (session.role != NetRole::Host) return true;
    const auto identity = reader.u64();
    const auto ready = reader.u8();
    if (!reader.finished() || ready > 1) return true;
    for (auto& [id, peer] : session.peers) {
        if (!peer.connected || peer.endpoint != datagram.from || peer.identity != identity) continue;
        peer.party_ready = ready != 0;
        peer.last_heard_ms = session.now_ms;
        return true;
    }
    return true;
}

bool start_network_party(NetSession& session) {
    if (session.role != NetRole::Host || !session.party_ready) return false;
    for (const auto& [id, peer] : session.peers)
        if (peer.connected && (!peer.party_ready || peer.snapshot.id != 0)) return false;
    session.match_started = true;
    network_event(session, "party_started");
    send_party_state(session);
    return true;
}
