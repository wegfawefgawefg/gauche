#pragma once

#include "net_codec.hpp"
#include "rollback.hpp"

#include <cstdint>
#include <span>

enum class WireKind : std::uint8_t {
    Hello = 1, Welcome, Input, Canonical, Correction,
    SnapshotChunk, SnapshotRequest, SnapshotAck, Heartbeat, PartyState, PartyReady,
};

constexpr std::uint32_t wire_magic = 0x47415543U;
constexpr std::uint16_t wire_version = 14;
constexpr std::uint64_t gameplay_version = 0x2026091647ULL;
// Six 176-byte canonical frames plus headers fit the relay's 1200-byte payload.
constexpr std::size_t canonical_frames_per_packet = 6;
constexpr std::size_t max_correction_chunks = 20;

PacketWriter begin_packet(WireKind kind);
bool read_packet_header(PacketReader& reader, WireKind& kind);
void write_frame(PacketWriter& writer, const CanonicalFrame& frame);
CanonicalFrame read_frame(PacketReader& reader);
std::uint64_t bytes_hash(std::span<const std::uint8_t> bytes);

Input missing_remote_input(const Game& game, int owner);
