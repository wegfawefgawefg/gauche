#pragma once

#include "net_codec.hpp"
#include "rollback.hpp"

#include <cstdint>
#include <span>

enum class WireKind : std::uint8_t {
    Hello = 1, Welcome, Input, Canonical, Correction,
    SnapshotChunk, SnapshotRequest, SnapshotAck, Heartbeat,
};

constexpr std::uint32_t wire_magic = 0x47415543U;
constexpr std::uint16_t wire_version = 11;
constexpr std::uint64_t gameplay_version = 0x20260914ECULL;

PacketWriter begin_packet(WireKind kind);
bool read_packet_header(PacketReader& reader, WireKind& kind);
void write_frame(PacketWriter& writer, const CanonicalFrame& frame);
CanonicalFrame read_frame(PacketReader& reader);
std::uint64_t bytes_hash(std::span<const std::uint8_t> bytes);

Input missing_remote_input(const Game& game, int owner);
