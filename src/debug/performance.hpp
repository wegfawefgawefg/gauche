#pragma once
#include <array>
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <string>

enum class PerfZone {
    Frame, Events, NetworkPump, Menu, Generation, Simulation, Tick, Temperature,
    Surfaces, Roofs, Props, Timers, Players, Actors, Hazards, Cleanup,
    NetworkStep, Catchup, Rollback, Hash, Snapshot, Audio, Cosmetics,
    Render, Lighting, Tiles, SurfaceDraw, PropDraw, EntityDraw, Particles,
    Overhead, Hud, Annotations, MenuDraw, DebugDraw, WindowBlit, Present, Sleep, Count
};
inline constexpr std::size_t perf_zone_count=static_cast<std::size_t>(PerfZone::Count);
inline constexpr std::array<const char*,perf_zone_count> perf_names{{
    "Frame total","Events / input","Network pump","Menu / tools","World generation",
    "Simulation loop","Game tick","  Temperature","  Surfaces","  Roofs","  Props / growth",
    "  Entity timers","  Players","  Actors / AI","  Currents / hazards","  Cleanup / reactor",
    "Network step","Client catchup","Rollback replay","Gameplay hash","Snapshot codec",
    "Audio update","Cosmetics update","World rendering","  Lighting","  Terrain",
    "  Surface drawing","  Prop drawing","  Entity drawing","  Particles",
    "  Overhead / shafts","  HUD / status","Debug annotations","Menu rendering",
    "ImGui / cursor","Window blit","Present / driver wait","Limiter sleep"
}};
struct PerfFrame {
    std::array<std::uint64_t,perf_zone_count> ns{};
    std::array<std::uint32_t,perf_zone_count> calls{};
    std::uint64_t sequence=0;
    double cpu_ms=0;
    int cap=0,vsync=0,width=0,height=0;
    bool focused=false,minimized=false;
};
struct Performance {
    static constexpr std::size_t history_limit=300;
    bool visible=false,enabled=false,recording=false;
    PerfFrame current;
    std::array<PerfFrame,history_limit> history{};
    std::size_t count=0,next=0;
    std::uint64_t frame_start=0,sequence=0;
    std::clock_t cpu_start=0;
    std::ofstream csv;
    int capture_left=0;
    std::filesystem::path directory;
    std::string status,renderer;
};
Performance& performance();
void begin_performance_frame(std::uint64_t start);
void end_performance_frame(int cap,int vsync,int width,int height,bool focused,bool minimized);
bool start_performance_capture(const std::filesystem::path& path,int frames);
void stop_performance_capture();
bool export_performance_history(const std::filesystem::path& path);
void draw_performance_panel();
struct PerfScope {
#if defined(TEEMING_PERF_ENABLED)
    PerfZone zone;
    std::uint64_t start=0;
    explicit PerfScope(PerfZone value);
    ~PerfScope(){stop();}
    void next(PerfZone value);
    void stop();
    PerfScope(const PerfScope&)=delete;
    PerfScope& operator=(const PerfScope&)=delete;
#else
    explicit PerfScope(PerfZone){}
    void next(PerfZone){}
    void stop(){}
#endif
};
