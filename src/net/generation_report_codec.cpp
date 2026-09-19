#include "generation_report_codec.hpp"
#if __has_include("generation_build.hpp")
#include "generation_build.hpp"
#else
#define TEEMING_GENERATOR_REVISION "revision unavailable"
#endif
#include <algorithm>
#include <array>

namespace {
void string(PacketWriter& w,const std::string& s) {w.u16(static_cast<std::uint16_t>(s.size()));w.bytes.insert(w.bytes.end(),s.begin(),s.end());}
std::string string(PacketReader& r) {
    const auto n=r.u16();
    if(!r.okay || n>1024 || n>r.bytes.size()-r.position){r.okay=false;return {};}
    std::string s(reinterpret_cast<const char*>(r.bytes.data()+r.position),n);r.position+=n;
    if(s.find('\0')!=std::string::npos)r.okay=false;
    return s;
}
Cell cell(PacketReader& r) {const Cell c=r.cell();if(c.x< -2048 || c.y< -2048 || c.x>2048 || c.y>2048)r.okay=false;return c;}
void cells(PacketWriter& w,const std::vector<Cell>& points,bool geometry) {
    w.u32(geometry ? static_cast<std::uint32_t>(points.size()) : 0);
    if(geometry)for(Cell c:points)w.cell(c);
}
std::vector<Cell> cells(PacketReader& r) {
    const auto n=r.u32();std::vector<Cell> points;
    if(!r.okay || n>65536 || n>(r.bytes.size()-r.position)/8){r.okay=false;return points;}
    points.reserve(n);for(std::uint32_t i=0;i<n&&r.okay;++i)points.push_back(cell(r));return points;
}
std::vector<std::uint8_t> encode(const GenerationReport& report,bool geometry) {
    PacketWriter w;w.u16(4);w.u64(report.seed);w.u64(report.initial_rng);w.i32(report.floor);
    string(w,report.revision.empty() ? TEEMING_GENERATOR_REVISION : report.revision);
    w.u64(report.inhabitants_seed);
    w.u8(static_cast<std::uint8_t>(report.themes.major));w.u8(static_cast<std::uint8_t>(report.themes.minor));
    w.u8(report.components_truncated);w.u8(!geometry);
    w.u16(static_cast<std::uint16_t>(report.features.size()));
    for(const auto& f:report.features) {
        w.u8(static_cast<std::uint8_t>(f.feature));w.u8(static_cast<std::uint8_t>(f.outcome));
        w.u32(f.roll);w.u32(f.denominator);w.i32(f.candidate_count);string(w,f.reason);string(w,f.variant);
        w.u16(geometry ? static_cast<std::uint16_t>(f.regions.size()) : 0);
        if(geometry)for(const auto& region:f.regions){w.cell(region.low);w.cell(region.high);}
    }
    w.u16(static_cast<std::uint16_t>(report.components.size()));
    for(const auto& c:report.components) {
        w.u8(static_cast<std::uint8_t>(c.feature));w.i32(c.parent);
        string(w,c.slot);string(w,c.choice);string(w,c.result);w.u32(c.ticket);w.u32(c.total);w.i32(c.placed);w.cell(c.anchor);
        w.u8(c.guide_closed);w.u8(c.guide_cell_centers);
        w.u8(c.cells_are_area);
        cells(w,c.cells,geometry);cells(w,c.guide,geometry);cells(w,c.rejected_cells,geometry);
        w.u16(static_cast<std::uint16_t>(c.options.size()));
        for(const auto& o:c.options){w.i32(o.value);string(w,o.name);w.u32(o.weight);}
    }
    return w.bytes;
}
}
std::vector<std::uint8_t> encode_generation_report(const GenerationReport& report,std::size_t budget) {
    budget=std::min(budget,generation_report_wire_limit);
    // Normal reports already have a bounded row count. Preserve every decision;
    // if necessary omit geometry as a whole, never silently truncate a path.
    if(report.features.size()>generation_rules.size() || report.components.size()>generation_component_limit)return {};
    auto bytes=encode(report,!report.geometry_omitted);
    if(bytes.size()>budget)bytes=encode(report,false);
    if(bytes.size()>budget)return {};
    return bytes;
}
std::shared_ptr<const GenerationReport> decode_generation_report(std::span<const std::uint8_t> bytes,const Game& game) {
    if(bytes.size()>generation_report_wire_limit)return {};
    PacketReader r{bytes};const auto version=r.u16();if(version<1 || version>4)return {};
    auto report=std::make_shared<GenerationReport>();report->received=true;
    report->seed=r.u64();report->initial_rng=r.u64();report->floor=r.i32();report->revision=string(r);
    if(version>=4)report->inhabitants_seed=r.u64();
    report->themes.major=static_cast<GenerationTheme>(r.u8());report->themes.minor=static_cast<GenerationTheme>(r.u8());
    const auto capped=r.u8(),omitted=r.u8();report->components_truncated=capped!=0;report->geometry_omitted=omitted!=0;
    if(!r.okay || capped>1 || omitted>1 || report->seed!=game.run.seed || report->floor!=game.run.floor ||
        report->themes.major>=GenerationTheme::Count || report->themes.minor>=GenerationTheme::Count)return {};
    const auto features=r.u16();if(features>generation_rules.size())return {};
    std::array<bool,static_cast<std::size_t>(GenerationFeature::Count)> seen{};
    for(int i=0;i<features&&r.okay;++i) {
        FeatureDecision f;f.feature=static_cast<GenerationFeature>(r.u8());f.outcome=static_cast<GenerationOutcome>(r.u8());
        if(f.feature>=GenerationFeature::Count || f.outcome>GenerationOutcome::Suppressed)return {};
        if(seen[static_cast<std::size_t>(f.feature)])return {};
        seen[static_cast<std::size_t>(f.feature)]=true;
        f.roll=r.u32();f.denominator=r.u32();f.candidate_count=r.i32();f.reason=string(r);f.variant=string(r);
        if(f.candidate_count< -1 || f.candidate_count>1'000'000 || f.denominator>1'000'000 ||
            (f.denominator && f.roll>=f.denominator))return {};
        const auto regions=r.u16();if(regions>1024 || (omitted&&regions))return {};
        for(int n=0;n<regions&&r.okay;++n) {
            const Cell low=cell(r),high=cell(r);
            if(high.x<low.x || high.y<low.y)return {};
            f.regions.push_back({low,high});
        }
        report->features.push_back(std::move(f));
    }
    const auto components=r.u16();if(components>generation_component_limit)return {};
    for(int i=0;i<components&&r.okay;++i) {
        ComponentDecision c;c.feature=static_cast<GenerationFeature>(r.u8());c.parent=r.i32();
        if(c.feature>=GenerationFeature::Count || c.parent< -1 || c.parent>=i)return {};
        if(c.parent>=0 && report->components[static_cast<std::size_t>(c.parent)].feature!=c.feature)return {};
        c.slot=string(r);c.choice=string(r);c.result=string(r);c.ticket=r.u32();c.total=r.u32();c.placed=r.i32();c.anchor=cell(r);
        const auto closed=r.u8(),centers=r.u8();if(closed>1 || centers>1 || c.placed<0 || c.placed>1'000'000)return {};
        c.guide_closed=closed!=0;c.guide_cell_centers=centers!=0;
        if(version>=3){const auto area=r.u8();if(area>1)return {};c.cells_are_area=area!=0;}
        c.cells=cells(r);c.guide=cells(r);c.rejected_cells=cells(r);
        // Legacy reports used guide presence to distinguish fills from markers.
        if(version<3)c.cells_are_area=!c.guide.empty();
        if(omitted && (!c.cells.empty() || !c.guide.empty() || !c.rejected_cells.empty()))return {};
        const auto options=r.u16();if(!r.okay || options>64 || c.total>1'000'000)return {};
        if(options ? c.ticket>=c.total : (version<2 || c.total!=0 || c.ticket!=0 || c.choice.empty()))return {};
        std::uint64_t sum=0;bool selected=false;
        for(int n=0;n<options&&r.okay;++n) {
            ComponentOption o;o.value=r.i32();o.name=string(r);o.weight=r.u32();
            if(c.ticket>=sum && c.ticket<sum+o.weight)selected=o.name==c.choice;
            sum+=o.weight;c.options.push_back(std::move(o));
        }
        if(sum!=c.total || (options && !selected))return {};
        report->components.push_back(std::move(c));
    }
    return r.finished() ? report : nullptr;
}
