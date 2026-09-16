#include "snapshot_codec.hpp"
#include "generation_report_codec.hpp"

std::vector<std::uint8_t> encode_network_snapshot(const Game& game) {
    const auto state=encode_game(game);
    if(state.size()+12>snapshot_wire_limit)return {};
    PacketWriter w;w.u32(0x31504e53);w.u32(static_cast<std::uint32_t>(state.size()));
    w.bytes.insert(w.bytes.end(),state.begin(),state.end());
    std::vector<std::uint8_t> diagnostics;
    if(GAUCHE_DEV_MODE && game.generation_report)
        diagnostics=encode_generation_report(*game.generation_report,snapshot_wire_limit-state.size()-12);
    w.u32(static_cast<std::uint32_t>(diagnostics.size()));w.bytes.insert(w.bytes.end(),diagnostics.begin(),diagnostics.end());
    return w.bytes;
}
bool decode_network_snapshot(std::span<const std::uint8_t> bytes,Game& game,std::string& error,std::string& diagnostic_note) {
    error.clear();diagnostic_note.clear();PacketReader r{bytes};
    if(bytes.size()>snapshot_wire_limit || r.u32()!=0x31504e53){error="Network snapshot envelope mismatch";return false;}
    const auto size=r.u32();
    if(!r.okay || size>bytes.size()-r.position){error="Truncated network snapshot state";return false;}
    const auto state=bytes.subspan(r.position,size);r.position+=size;
    const auto report_size=r.u32();
    if(!r.okay || report_size>generation_report_wire_limit || report_size!=bytes.size()-r.position){error="Invalid snapshot attachment size";return false;}
    Game restored;if(!decode_game(state,restored,error))return false;
    if(report_size) {
        restored.generation_report=decode_generation_report(bytes.subspan(r.position,report_size),restored);
        if(!restored.generation_report)diagnostic_note="Generation diagnostics rejected; game state received";
    }
    game=std::move(restored);return true;
}
