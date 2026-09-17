#include "performance.hpp"
#include "generation_build.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <iomanip>

namespace {
void header(std::ostream& out) {
    out<<"# Gauche "<<GAUCHE_GENERATOR_REVISION<<"; inclusive main-thread wall timings, not GPU timings; process CPU may include other threads\n";
    out<<"# Platform "<<SDL_GetPlatform()<<"; renderer "<<performance().renderer<<"; logical CPUs "<<SDL_GetNumLogicalCPUCores()<<'\n';
    out<<"frame,cap_fps,vsync,render_width,render_height,focused,minimized,process_cpu_ms";
    for(const char* name:perf_names)out<<",\""<<name<<" ms\",\""<<name<<" calls\"";
    out<<'\n';
}
void row(std::ostream& out,const PerfFrame& f) {
    out<<f.sequence<<','<<f.cap<<','<<f.vsync<<','<<f.width<<','<<f.height<<','<<f.focused<<','<<f.minimized<<','<<std::fixed<<std::setprecision(4)<<f.cpu_ms;
    for(std::size_t i=0;i<perf_zone_count;++i)out<<','<<static_cast<double>(f.ns[i])*1e-6<<','<<f.calls[i];
    out<<'\n';
}
}
Performance& performance(){static Performance state;return state;}
PerfScope::PerfScope(PerfZone value):zone(value) {if(performance().recording)start=SDL_GetTicksNS();}
void PerfScope::stop() {
    if(!start)return;
    auto& p=performance();
    if(p.recording){const auto i=static_cast<std::size_t>(zone);p.current.ns[i]+=SDL_GetTicksNS()-start;++p.current.calls[i];}
    start=0;
}
void PerfScope::next(PerfZone value){stop();zone=value;if(performance().recording)start=SDL_GetTicksNS();}
void begin_performance_frame(std::uint64_t start) {
    auto& p=performance();p.recording=p.enabled || p.csv.is_open();
    if(!p.recording)return;
    p.current={};p.current.sequence=++p.sequence;p.frame_start=start;p.cpu_start=std::clock();
}
void end_performance_frame(int cap,int vsync,int width,int height,bool focused,bool minimized) {
    auto& p=performance();if(!p.recording)return;
    auto& frame=p.current;
    frame.ns[0]=SDL_GetTicksNS()-p.frame_start;frame.calls[0]=1;
#ifdef __EMSCRIPTEN__
    // Emscripten maps process CPU clocks to monotonic elapsed time.
    frame.cpu_ms=-1;
#else
    const auto cpu=std::clock();frame.cpu_ms=cpu!=std::clock_t{-1} && p.cpu_start!=std::clock_t{-1} ?
        1000.0*static_cast<double>(cpu-p.cpu_start)/CLOCKS_PER_SEC : -1;
#endif
    frame.cap=cap;frame.vsync=vsync;frame.width=width;frame.height=height;frame.focused=focused;frame.minimized=minimized;
    p.history[p.next]=frame;p.next=(p.next+1)%p.history.size();p.count=std::min(p.count+1,p.history.size());
    p.recording=false;
    if(p.csv.is_open()) {
        row(p.csv,frame);
        if(!p.csv){p.csv.close();p.capture_left=0;p.status="Capture write failed";}
        else if(--p.capture_left<=0)stop_performance_capture();
    }
}
bool start_performance_capture(const std::filesystem::path& path,int frames) {
    auto& p=performance();stop_performance_capture();p.csv.clear();
    p.csv.open(path);if(!p.csv){p.status="Could not open "+path.string();return false;}
    header(p.csv);p.capture_left=std::clamp(frames,1,36000);p.status=path.string();return true;
}
void stop_performance_capture(){auto& p=performance();if(p.csv.is_open()){p.csv.flush();const bool okay=static_cast<bool>(p.csv);p.csv.close();if(!okay)p.status="Capture write failed";}p.capture_left=0;}
bool export_performance_history(const std::filesystem::path& path) {
    auto& p=performance();std::ofstream out(path);if(!out){p.status="Could not open "+path.string();return false;}
    header(out);const auto first=(p.next+p.history.size()-p.count)%p.history.size();
    for(std::size_t i=0;i<p.count;++i)row(out,p.history[(first+i)%p.history.size()]);
    out.flush();p.status=out ? path.string() : "Export write failed";return static_cast<bool>(out);
}
