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
    bool rooms=true, changes=false, actor_changes=false, follow_step=true, keep_view_on_regen=false;
    GenerationTraceOptions capture_options;
    int floor=1, checkpoint=0, selected_feature=-1, selected_component=-1;
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
void select_worldgen_checkpoint(WorldGenViewer& viewer,int checkpoint);
void recapture_worldgen(WorldGenViewer& viewer);
void draw_worldgen_changes(SDL_Renderer* renderer,const WorldGenViewer& viewer,const Game& before,const Game& after);
void process_worldgen_requests(MenuShell& menu);
bool worldgen_event(const SDL_Event& event);
void update_worldgen(float seconds);
void draw_worldgen(SDL_Renderer* renderer,const GameGraphics& graphics);
void draw_worldgen_details(const Game& game);
void draw_generation_report(const GenerationReport& report,bool inspection);
void draw_live_generation_details(const Game& game);

void draw_component_decisions(const GenerationReport& report,GenerationFeature feature,bool inspection);
