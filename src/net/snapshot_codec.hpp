#pragma once
#include "../net_codec.hpp"
inline constexpr std::size_t snapshot_chunk_bytes=900;
inline constexpr std::size_t snapshot_chunk_limit=2048;
inline constexpr std::size_t snapshot_wire_limit=snapshot_chunk_bytes*snapshot_chunk_limit;
std::vector<std::uint8_t> encode_network_snapshot(const Game& game);
bool decode_network_snapshot(std::span<const std::uint8_t> bytes,Game& game,std::string& error,std::string& diagnostic_note);
