#include "net_protocol.hpp"

PacketWriter begin_packet(WireKind kind) {
    PacketWriter writer;
    writer.u32(wire_magic);
    writer.u16(wire_version);
    writer.u8(static_cast<std::uint8_t>(kind));
    return writer;
}

bool read_packet_header(PacketReader& reader, WireKind& kind) {
    if (reader.u32() != wire_magic || reader.u16() != wire_version) return false;
    const std::uint8_t raw = reader.u8();
    if (raw < static_cast<std::uint8_t>(WireKind::Hello) ||
        raw > static_cast<std::uint8_t>(WireKind::Heartbeat)) return false;
    kind = static_cast<WireKind>(raw);
    return reader.okay;
}

void write_frame(PacketWriter& writer, const CanonicalFrame& frame) {
    writer.u64(frame.tick);
    writer.u64(frame.hash);
    for (const Input& input : frame.inputs) writer.input(input);
}

CanonicalFrame read_frame(PacketReader& reader) {
    CanonicalFrame frame;
    frame.tick = reader.u64();
    frame.hash = reader.u64();
    for (Input& input : frame.inputs) input = reader.input();
    return frame;
}

std::uint64_t bytes_hash(std::span<const std::uint8_t> bytes) {
    std::uint64_t hash = 1469598103934665603ULL;
    for (std::uint8_t byte : bytes) {
        hash ^= byte;
        hash *= 1099511628211ULL;
    }
    return hash;
}
