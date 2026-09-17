#include "fragment.hpp"
#include "../net_session_internal.hpp"
#include <algorithm>

namespace {
void send_datagram(NetSession& session, NetEndpoint to, const PacketWriter& packet) {
    std::string error;
    if (!send_traversal(session, to, packet.bytes, error)) {
        session.status = error;
        network_event(session, "send_failed");
    } else {
        ++session.diagnostics.sent_packets;
        session.diagnostics.sent_bytes += packet.bytes.size();
    }
}
}

void send_fragmented(NetSession& session, NetEndpoint to, const PacketWriter& packet) {
    if (packet.bytes.size() <= game_datagram_bytes) { send_datagram(session,to,packet); return; }
    if (packet.bytes.size() > message_byte_limit) {
        session.status="Network message exceeds byte budget"; network_event(session,"message_too_large"); return;
    }
    const auto checksum=bytes_hash(packet.bytes);
    for (std::size_t start=0; start<packet.bytes.size(); start+=fragment_payload_bytes) {
        auto part=begin_packet(WireKind::Fragment);
        part.u64(checksum); part.u32(static_cast<std::uint32_t>(packet.bytes.size()));
        part.u32(static_cast<std::uint32_t>(start/fragment_payload_bytes));
        const auto end=std::min(start+fragment_payload_bytes,packet.bytes.size());
        part.bytes.insert(part.bytes.end(),packet.bytes.begin()+static_cast<std::ptrdiff_t>(start),
                          packet.bytes.begin()+static_cast<std::ptrdiff_t>(end));
        send_datagram(session,to,part);
    }
}

bool receive_fragment(NetSession& session, Datagram& datagram, PacketReader& reader) {
    // Unknown endpoints must complete the small Hello handshake before allocating assemblies.
    if (session.role==NetRole::Client) {
        if (datagram.from!=session.host_endpoint) return false;
    } else {
        const bool known=std::any_of(session.peers.begin(),session.peers.end(),[&](const auto& entry) {
            return entry.second.connected && entry.second.endpoint==datagram.from;
        });
        if (!known) return false;
    }
    const auto checksum=reader.u64(); const auto total=reader.u32(); const auto index=reader.u32();
    if (!reader.okay || total<=game_datagram_bytes || total>message_byte_limit) return false;
    const auto count=(total+fragment_payload_bytes-1)/fragment_payload_bytes;
    if (index>=count) return false;
    const auto offset=static_cast<std::size_t>(index)*fragment_payload_bytes;
    const auto size=std::min(fragment_payload_bytes,static_cast<std::size_t>(total)-offset);
    if (reader.bytes.size()-reader.position!=size) return false;
    auto& transfers=session.fragments;
    std::erase_if(transfers,[&](const auto& t){ return t.expires_ms<=session.now_ms; });
    auto found=std::find_if(transfers.begin(),transfers.end(),[&](const auto& t) {
        return t.from==datagram.from && t.checksum==checksum;
    });
    if (found==transfers.end()) {
        std::size_t allocated=0; for (const auto& t:transfers) allocated+=t.total;
        if (transfers.size()>=128 || allocated+total>4*message_byte_limit) return false;
        transfers.push_back({datagram.from,checksum,session.now_ms+2000,total,
                             std::vector<std::uint8_t>(total),std::vector<bool>(count),0});
        found=transfers.end()-1;
    }
    if (found->total!=total) return false;
    if (!found->present[index]) {
        std::copy(reader.bytes.begin()+static_cast<std::ptrdiff_t>(reader.position),reader.bytes.end(),
                  found->bytes.begin()+static_cast<std::ptrdiff_t>(offset));
        found->present[index]=true; ++found->received;
    }
    if (found->received!=count) return false;
    auto complete=std::move(found->bytes); transfers.erase(found);
    if (bytes_hash(complete)!=checksum) return false;
    datagram.bytes=std::move(complete);
    return true;
}
