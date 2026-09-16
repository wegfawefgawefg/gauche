#include "generation_trace.hpp"
#include <algorithm>

namespace {
std::size_t estimate(const Game& game,const FloorPlan& plan,const std::string& name) {
    std::size_t bytes=sizeof(GenerationCheckpoint)+sizeof(Game)+name.size()+1+
        game.stage.tiles.size()*sizeof(Tile)+game.stage.roofs.size()*sizeof(RoofSpan)+
        game.flight_contacts.size()*sizeof(FlightContact)+game.reactor_front.size()*sizeof(ReactorEvent)+
        game.boiler_feeds.size()*sizeof(BoilerFeed)+game.industrial_shifts.size()*sizeof(IndustrialShift)+
        game.lava_vents.size()*sizeof(LavaVent)+game.fissures.size()*sizeof(Fissure)+plan.rooms.size()*sizeof(RoomPlan);
    for (const auto& feed:game.boiler_feeds) bytes+=feed.belts.size()*sizeof(Cell);
    bytes+=plan.report.features.size()*sizeof(FeatureDecision)+plan.report.components.size()*sizeof(ComponentDecision);
    for (const auto& feature:plan.report.features) bytes+=feature.reason.size()+feature.variant.size()+2+feature.regions.size()*sizeof(GenerationRegion);
    for (const auto& child:plan.report.components) {
        bytes+=child.slot.size()+child.choice.size()+child.result.size()+3+
            (child.cells.size()+child.guide.size()+child.rejected_cells.size())*sizeof(Cell)+child.options.size()*sizeof(ComponentOption);
        for (const auto& option:child.options) bytes+=option.name.size()+1;
    }
    return bytes;
}
}
void GenerationTrace::reset() {
    const auto saved=options;*this={};options=saved;
    options.every=std::clamp(options.every,1,100);
}
void GenerationTrace::capture(const char* name,const Game& game,const FloorPlan& plan) {
    if (coarse_count>=limit) {truncated=true;return;}
    bytes+=estimate(game,plan,name);++coarse_count;
    checkpoints.push_back({name,std::make_unique<Game>(game),plan.rooms,plan.report,false,-1});
}
void GenerationTrace::capture_detail(const char* name,GenerationFeature feature,int component,const Game& game,const FloorPlan& plan) {
    if (!options.details || (options.feature>=0 && options.feature!=static_cast<int>(feature))) return;
    ++detail_seen;
    if ((detail_seen-1)%static_cast<std::size_t>(std::clamp(options.every,1,100))) return;
    const std::string label=std::string(name)+" #"+std::to_string(detail_seen);
    const auto size=estimate(game,plan,label);
    if (detail_count>=detail_limit || detail_bytes+size>detail_byte_limit) {detail_truncated=true;return;}
    bytes+=size;detail_bytes+=size;++detail_count;
    checkpoints.push_back({label,std::make_unique<Game>(game),plan.rooms,plan.report,true,component});
}
