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
        raw > static_cast<std::uint8_t>(WireKind::Fragment)) return false;
    kind = static_cast<WireKind>(raw);
    return reader.okay;
}

void write_frame(PacketWriter& writer, const CanonicalFrame& frame) {
    writer.u64(frame.tick);
    writer.u64(frame.hash);
    writer.u32(static_cast<std::uint32_t>(frame.inputs.size()));
    for (const auto& [id, input] : frame.inputs) { writer.i32(id); writer.input(input); }
}

CanonicalFrame read_frame(PacketReader& reader) {
    CanonicalFrame frame;
    frame.tick = reader.u64();
    frame.hash = reader.u64();
    const auto count = reader.u32();
    // One ID plus one fixed-size input; validate before allocating.
    if (!reader.okay || count > (reader.bytes.size()-reader.position)/44) { reader.okay=false; return frame; }
    PlayerId previous = -1;
    for (std::uint32_t index=0; index<count; ++index) {
        const auto id=reader.i32();
        if (id<0 || id<=previous) { reader.okay=false; return frame; }
        previous=id; frame.inputs.emplace(id,reader.input());
    }
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
    if (!has_player(game, owner)) return input;
    const Entity* player = get_entity(game, player_state(game, owner).controlled);
    // RELEASE: A missing packet must not invent a bow release and a projectile.
    input.use = player != nullptr && player->inventory.held()->kind == ItemKind::Bow && player->counter_a > 0;
    return input;
}
