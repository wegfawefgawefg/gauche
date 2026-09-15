#include "traversal.hpp"
#include "../net_session.hpp"

#include <gubsy/realnet/rendezvous.hpp>
#include <gubsy/realnet/relay.hpp>

#include <algorithm>

namespace {

void send_control(NetSession& session, NetEndpoint target, std::string bytes) {
    if (target.port == 0 || bytes.empty()) return;
    std::string error;
    if (!session.socket.send(target,
            {reinterpret_cast<const std::uint8_t*>(bytes.data()), bytes.size()}, error))
        network_event(session, "traversal_send_failed");
}

void punch_packet(NetSession& session, realnet::PacketKind kind, NetEndpoint target,
                  const std::string& attempt, const std::string& secret) {
    auto& transport = session.traversal;
    realnet::Packet packet;
    packet.kind = kind;
    packet.role = transport.host ? "host" : "joiner";
    packet.room_code = transport.room;
    packet.join_attempt_id = attempt;
    packet.seq = transport.sequence++;
    packet.ts_ms = realnet::unix_time_ms();
    realnet::sign_packet(packet, secret);
    send_control(session, target, realnet::encode_packet(packet));
}

realnet::RelayPacket relay_packet(NetSession& session, realnet::RelayPacketKind kind) {
    auto& transport = session.traversal;
    realnet::RelayPacket packet;
    packet.kind = kind;
    packet.role = transport.host ? realnet::RelayRole::Host : realnet::RelayRole::Joiner;
    packet.room_code = transport.room;
    packet.join_attempt_id = transport.attempt;
    packet.allocation_id = transport.allocation;
    packet.seq = transport.sequence++;
    packet.ts_ms = realnet::unix_time_ms();
    return packet;
}

const std::string& relay_key(const Traversal& transport) {
    return transport.host ? transport.host_secret : transport.relay_secret;
}

TraversalRoute* route_for(Traversal& transport, const std::string& attempt) {
    if (attempt.empty()) return nullptr;
    for (auto& route : transport.routes) if (route.attempt == attempt) return &route;
    // CAPACITY: Keep stable virtual endpoints for a run; old datagrams cannot become another peer.
    if (transport.routes.size() >= 128) return nullptr;
    transport.routes.push_back({});
    auto& route = transport.routes.back();
    route.attempt = attempt;
    route.relayed = {0, static_cast<std::uint16_t>(transport.routes.size()), true};
    return &route;
}

bool recent(std::uint64_t stamp) {
    const auto now = realnet::unix_time_ms();
    return stamp <= now + 5000 && stamp + 30000 >= now;
}

void receive_punch(NetSession& session, const Datagram& datagram) {
    auto& transport = session.traversal;
    realnet::Packet packet;
    std::string error;
    if (!realnet::decode_packet(std::string(datagram.bytes.begin(), datagram.bytes.end()), packet, error) ||
        packet.room_code != transport.room || !recent(packet.ts_ms)) return;
    if (!transport.host && packet.join_attempt_id != transport.attempt) return;
    if (packet.kind == realnet::PacketKind::EndpointHint) {
        const auto& key = transport.host ? transport.host_secret : transport.punch_secret;
        if (datagram.from != transport.punch_server || key.empty() ||
            !realnet::verify_packet(packet, key) || !packet.peer_endpoint) return;
        auto* route = route_for(transport, packet.join_attempt_id);
        if (route == nullptr) return;
        if (!resolve_endpoint(packet.peer_endpoint->host, packet.peer_endpoint->port, route->direct, error)) return;
        route->punch_secret = transport.host ? packet.punch_secret : transport.punch_secret;
        route->expires_ms = session.now_ms + 30000;
        return;
    }
    if (packet.kind != realnet::PacketKind::PunchProbe && packet.kind != realnet::PacketKind::PunchAck) return;
    for (auto& route : transport.routes) {
        if (route.attempt != packet.join_attempt_id || route.punch_secret.empty() ||
            route.expires_ms < session.now_ms || !realnet::verify_packet(packet, route.punch_secret)) continue;
        route.direct = datagram.from;
        route.authenticated = true;
        if (packet.kind == realnet::PacketKind::PunchProbe)
            punch_packet(session, realnet::PacketKind::PunchAck, datagram.from, route.attempt, route.punch_secret);
        if (!transport.host && !transport.force_relay && transport.phase == TraversalPhase::Punch) {
            session.host_endpoint = datagram.from;
            session.next_hello_ms = 0;
            transport.phase = TraversalPhase::Connected;
            network_event(session, "punch_connected");
        }
        return;
    }
}

bool receive_relay(NetSession& session, Datagram& datagram) {
    auto& transport = session.traversal;
    realnet::RelayPacket packet;
    std::string error;
    if (datagram.from != transport.relay_server ||
        !realnet::decode_relay_packet(std::string(datagram.bytes.begin(), datagram.bytes.end()), packet, error) ||
        packet.room_code != transport.room || relay_key(transport).empty() ||
        !realnet::verify_relay_packet(packet, relay_key(transport)) || !recent(packet.ts_ms)) return true;
    if (!transport.host && (packet.allocation_id != transport.allocation ||
        packet.join_attempt_id != transport.attempt)) return true;
    auto* route = route_for(transport, packet.join_attempt_id);
    if (route == nullptr) return true;
    if (packet.kind == realnet::RelayPacketKind::Ready) {
        route->allocation = packet.allocation_id;
        transport.relay_ready = true;
        if (!transport.host && transport.phase == TraversalPhase::Relay) {
            session.host_endpoint = route->relayed;
            session.next_hello_ms = 0;
            transport.phase = TraversalPhase::Connected;
            network_event(session, "relay_connected");
        }
        return true;
    }
    if (packet.kind != realnet::RelayPacketKind::Data || packet.payload.size() > 1200 ||
        route->allocation.empty() || route->allocation != packet.allocation_id) return true;
    datagram.from = route->relayed;
    datagram.bytes = std::move(packet.payload);
    return false;
}

} // namespace

void step_traversal(NetSession& session) {
    auto& transport = session.traversal;
    if (transport.phase == TraversalPhase::Off || transport.phase == TraversalPhase::Failed) return;
    if (transport.deadline_ms == 0) transport.deadline_ms = session.now_ms + 3000;
    if (!transport.host && transport.phase == TraversalPhase::Punch &&
        (transport.force_relay || session.now_ms >= transport.deadline_ms)) {
        transport.phase = TraversalPhase::Relay;
        transport.deadline_ms = session.now_ms + 6000;
        network_event(session, "relay_fallback");
    }
    if (!transport.host && transport.phase == TraversalPhase::Relay && session.now_ms >= transport.deadline_ms) {
        transport.phase = TraversalPhase::Failed;
        session.status = "Direct connection and relay failed; retry joining the room";
        network_event(session, "traversal_failed");
        return;
    }
    if (session.now_ms >= transport.next_hello_ms) {
        transport.next_hello_ms = session.now_ms + 1000;
        punch_packet(session, transport.host ? realnet::PacketKind::HostHello : realnet::PacketKind::JoinerHello,
            transport.punch_server, transport.attempt, transport.host ? transport.host_secret : transport.punch_secret);
    }
    if (session.now_ms >= transport.next_probe_ms) {
        transport.next_probe_ms = session.now_ms + 100;
        for (auto& route : transport.routes)
            if (!route.punch_secret.empty() && route.expires_ms >= session.now_ms)
                punch_packet(session, realnet::PacketKind::PunchProbe, route.direct, route.attempt, route.punch_secret);
    }
    if ((transport.host || transport.phase == TraversalPhase::Relay || session.host_endpoint.relayed) &&
        session.now_ms >= transport.next_relay_ms) {
        transport.next_relay_ms = session.now_ms + 1000;
        auto packet = relay_packet(session, transport.relay_ready ? realnet::RelayPacketKind::Keepalive : realnet::RelayPacketKind::Hello);
        realnet::sign_relay_packet(packet, relay_key(transport));
        send_control(session, transport.relay_server, realnet::encode_relay_packet(packet));
    }
}

bool receive_traversal(NetSession& session, Datagram& datagram) {
    if (session.traversal.phase == TraversalPhase::Off || datagram.bytes.empty()) return false;
    if (datagram.bytes.front() == '{') { receive_punch(session, datagram); return true; }
    if (datagram.bytes.size() >= 4 && datagram.bytes[0] == 'G' && datagram.bytes[1] == 'R' &&
        datagram.bytes[2] == 'L' && datagram.bytes[3] == 'Y') return receive_relay(session, datagram);
    return false;
}

bool authorized_route(const NetSession& session, NetEndpoint source) {
    if (session.traversal.phase == TraversalPhase::Off) return true;
    for (const auto& route : session.traversal.routes)
        if ((source.relayed && !route.allocation.empty() && source == route.relayed) ||
            (!source.relayed && route.authenticated && source == route.direct)) return true;
    return false;
}

bool send_traversal(NetSession& session, NetEndpoint target,
                    const std::vector<std::uint8_t>& bytes, std::string& error) {
    if (!target.relayed) return session.socket.send(target, bytes, error);
    for (const auto& route : session.traversal.routes) {
        if (route.relayed != target || route.allocation.empty()) continue;
        auto packet = relay_packet(session, realnet::RelayPacketKind::Data);
        packet.join_attempt_id = route.attempt;
        packet.allocation_id = route.allocation;
        packet.payload = bytes;
        realnet::sign_relay_packet(packet, relay_key(session.traversal));
        const std::string encoded = realnet::encode_relay_packet(packet);
        if (encoded.empty() || encoded.size() > realnet::kMaxRelayPacketBytes) break;
        return session.socket.send(session.traversal.relay_server,
            {reinterpret_cast<const std::uint8_t*>(encoded.data()), encoded.size()}, error);
    }
    error = "Relay route is not ready";
    return false;
}

const char* traversal_status(const NetSession& session) {
    switch (session.traversal.phase) {
    case TraversalPhase::Off: return "Direct UDP";
    case TraversalPhase::Punch: return session.traversal.host ? "Room host" : "Connecting directly";
    case TraversalPhase::Relay: return "Trying relay";
    case TraversalPhase::Connected: return session.host_endpoint.relayed ? "Relay" : "Direct (NAT punch)";
    case TraversalPhase::Failed: return "Connection failed";
    }
    return "Unknown";
}
