#pragma once
#include "../net_codec.hpp"
#include "../world/generation_report.hpp"

inline constexpr std::size_t generation_report_wire_limit=256U*1024U;
std::vector<std::uint8_t> encode_generation_report(const GenerationReport& report,std::size_t budget=generation_report_wire_limit);
std::shared_ptr<const GenerationReport> decode_generation_report(std::span<const std::uint8_t> bytes,const Game& game);
