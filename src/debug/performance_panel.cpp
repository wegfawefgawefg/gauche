#include "performance.hpp"
#include "playtest.hpp"
#include "generation_build.hpp"
#include <SDL3/SDL.h>
#include <imgui.h>
#include <algorithm>
#include <cmath>

namespace {
struct Stats {double mean=0,p95=0,peak=0,calls=0;};
std::filesystem::path output_path(const char* suffix) {
    const auto& p=performance();return p.directory/("performance-"+std::to_string(SDL_GetTicksNS())+suffix);
}
}
void draw_performance_panel() {
    auto& p=performance();if(!p.visible)return;
    ImGui::SetNextWindowSize({700,650},ImGuiCond_FirstUseEver);
    if(ImGui::Begin("Performance profiler",&p.visible)) {
        ImGui::Checkbox("Collect timings",&p.enabled);ImGui::SameLine();
        if(ImGui::Button("Clear history"))p.count=p.next=0;
        if(ImGui::Button("Save last 300 frames"))export_performance_history(output_path("-history.csv"));
        ImGui::SameLine();
        if(p.csv.is_open()) {if(ImGui::Button("Stop CSV capture"))stop_performance_capture();}
        else if(ImGui::Button("Record next 600 frames"))start_performance_capture(output_path(".csv"),600);
        if(p.capture_left)ImGui::Text("CSV: %d frames remaining",p.capture_left);
        if(!p.status.empty())ImGui::TextWrapped("%s",p.status.c_str());
        ImGui::Checkbox("Pause offline simulation while F1 is open",&playtest_tools().pause);
        static bool hide_inactive=true;
        ImGui::Checkbox("Hide inactive phases",&hide_inactive);
        ImGui::Text("Build %s", TEEMING_GENERATOR_REVISION);
        ImGui::TextWrapped("Inclusive main-thread wall times; nested rows overlap and must not be added together. Present includes command submission and driver/V-sync waits, not GPU execution time. Short phases may fall below the browser clock resolution.");
        if(p.count) {
            const auto& last=p.history[(p.next+p.history.size()-1)%p.history.size()];
            ImGui::Text("Cap %d FPS (0 = off) | SDL V-sync %d | render %dx%d",last.cap,last.vsync,last.width,last.height);
            ImGui::Text("%s / %s | %zu frames retained",last.focused ? "Focused" : "Background",last.minimized ? "minimized" : "visible",p.count);
            ImGui::Text("Renderer: %s",p.renderer.c_str());
#ifdef __EMSCRIPTEN__
            ImGui::TextUnformatted("Browser animation scheduling; cap 0 follows display refresh.");
#else
            if(!last.cap && !last.vsync)ImGui::TextUnformatted("Uncapped rendering: set a frame cap in Display settings to reduce work.");
#endif
            std::array<float,Performance::history_limit> plot{};
            static std::array<Stats,perf_zone_count> stats{};
            static double cpu_mean=0;
            static std::uint64_t refreshed=0,last_sequence=0;
            const auto now=SDL_GetTicksNS();
            const auto first=(p.next+p.history.size()-p.count)%p.history.size();
            float maximum=16.67F;
            for(std::size_t i=0;i<p.count;++i){plot[i]=static_cast<float>(p.history[(first+i)%p.history.size()].ns[0])*1e-6F;maximum=std::max(maximum,plot[i]);}
            if(!refreshed || (last.sequence!=last_sequence && now-refreshed>=250'000'000)) {
                stats={};cpu_mean=0;
                for(std::size_t zone=0;zone<perf_zone_count;++zone) {
                    std::array<double,Performance::history_limit> values{};
                    for(std::size_t i=0;i<p.count;++i){const auto& f=p.history[(first+i)%p.history.size()];values[i]=static_cast<double>(f.ns[zone])*1e-6;stats[zone].mean+=values[i];stats[zone].calls+=f.calls[zone];if(zone==0)cpu_mean+=f.cpu_ms;}
                    auto& s=stats[zone];s.mean/=static_cast<double>(p.count);s.calls/=static_cast<double>(p.count);
                    std::sort(values.begin(),values.begin()+static_cast<std::ptrdiff_t>(p.count));s.peak=values[p.count-1];s.p95=values[(p.count*95+99)/100-1];
                }
                cpu_mean/=static_cast<double>(p.count);refreshed=now;last_sequence=last.sequence;
            }
            const auto present=static_cast<std::size_t>(PerfZone::Present),sleep=static_cast<std::size_t>(PerfZone::Sleep);
            ImGui::Text("Avg frame %.2f ms | outside present/sleep %.2f | present %.2f | sleep %.2f",stats[0].mean,
                std::max(0.0,stats[0].mean-stats[present].mean-stats[sleep].mean),stats[present].mean,stats[sleep].mean);
            if(cpu_mean<0)ImGui::TextUnformatted("Process CPU usage unavailable in browser; these are elapsed timings.");
            else ImGui::Text("Process CPU %.2f ms/frame | about %.0f%% of one core",cpu_mean,stats[0].mean>0 ? cpu_mean/stats[0].mean*100 : 0);
            ImGui::PlotLines("Frame ms",plot.data(),static_cast<int>(p.count),0,nullptr,0,maximum,{0,65});
            if(ImGui::BeginTable("timings",5,ImGuiTableFlags_Borders|ImGuiTableFlags_RowBg|ImGuiTableFlags_ScrollY,{0,std::max(80.0F,ImGui::GetContentRegionAvail().y)})) {
                ImGui::TableSetupColumn("Phase (inclusive)",ImGuiTableColumnFlags_WidthStretch,2.4F);
                for(const char* name:{"Avg ms","P95 ms","Max ms","Calls/frame"})ImGui::TableSetupColumn(name,ImGuiTableColumnFlags_WidthStretch,1);
                ImGui::TableSetupScrollFreeze(0,1);
                ImGui::TableHeadersRow();
                for(std::size_t i=0;i<stats.size();++i){const auto& s=stats[i];if(hide_inactive && s.calls==0)continue;ImGui::TableNextRow();ImGui::TableNextColumn();ImGui::TextUnformatted(perf_names[i]);for(double value:{s.mean,s.p95,s.peak,s.calls}){ImGui::TableNextColumn();ImGui::Text("%.3f",value);}}
                ImGui::EndTable();
            }
        }else ImGui::TextUnformatted("Enable collection, then play. F1 can close without stopping collection.");
    }ImGui::End();
}
