#pragma once
#include "../net_socket.hpp"
#include <cstdint>

struct NetSession;
struct PacketWriter;
struct PacketReader;
inline constexpr std::size_t game_datagram_bytes = 1200;
inline constexpr std::size_t fragment_payload_bytes = 1160;
inline constexpr std::size_t message_byte_limit = 2 * 1024 * 1024;
struct FragmentReceive {
    NetEndpoint from{};
    std::uint64_t checksum = 0, expires_ms = 0;
    std::uint32_t total = 0;
    std::vector<std::uint8_t> bytes;
    std::vector<bool> present;
    std::size_t received = 0;
};
void send_fragmented(NetSession& session, NetEndpoint to, const PacketWriter& packet);
bool receive_fragment(NetSession& session, Datagram& datagram, PacketReader& reader);
