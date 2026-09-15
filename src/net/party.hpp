#pragma once

#include <cstdint>

struct NetSession;
struct Datagram;
struct PacketReader;
enum class WireKind : std::uint8_t;

void send_party_state(NetSession& session);
bool receive_party_state(NetSession& session, const Datagram& datagram,
                         PacketReader& reader, WireKind kind);
bool start_network_party(NetSession& session);
