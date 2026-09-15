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
        raw > static_cast<std::uint8_t>(WireKind::PartyReady)) return false;
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

Input missing_remote_input(const Game& game, int owner) {
    Input input;
    if (owner < 0 || owner >= static_cast<int>(game.players.size())) return input;
    const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
    // RELEASE: A missing packet must not invent a bow release and a projectile.
    input.use = player != nullptr && player->inventory.held()->kind == ItemKind::Bow && player->counter_a > 0;
    return input;
}
