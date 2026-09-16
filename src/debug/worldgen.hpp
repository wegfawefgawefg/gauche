#pragma once
#include "../world/generation_trace.hpp"
#include "../world/population_report.hpp"
#include "../render.hpp"

struct MenuShell;
// Local tooling only. Never copied to network rollback or save snapshots.
struct WorldGenViewer {
    bool active=false, playing=false, open_requested=false, return_requested=false;
    bool exit_requested=false;
    bool regenerate_requested=false, play_requested=false, details=true;
    bool rooms=true, changes=false;
    int floor=1, checkpoint=0;
    std::uint64_t seed=1;
    float zoom=1.0F;
    WorldRenderOptions render{};
    std::unique_ptr<Game> original;
    GenerationTrace trace;
    PopulationReport population;
};
WorldGenViewer& worldgen_viewer();
void regenerate_worldgen(WorldGenViewer& viewer);
void fit_worldgen(WorldGenViewer& viewer);
void process_worldgen_requests(MenuShell& menu);
bool worldgen_event(const SDL_Event& event);
void update_worldgen(float seconds);
void draw_worldgen(SDL_Renderer* renderer,const GameGraphics& graphics);
void draw_worldgen_details();
