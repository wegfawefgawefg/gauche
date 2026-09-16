#pragma once
#include "../render.hpp"
#include "../world/generation_report.hpp"
#include <memory>

// Presentation only. Weak ownership avoids retaining a discarded floor report;
// selection indices never cross between preview checkpoints and live reports.
struct LiveGenerationInspector {
    std::weak_ptr<const GenerationReport> report;
    int selected_feature=-1,selected_component=-1;
    bool overlay=false;
};
LiveGenerationInspector& live_generation_inspector();
LiveGenerationInspector& bind_live_generation_report(const Game& game);
void draw_generation_annotations(SDL_Renderer* renderer,const GenerationReport& report,
    int selected_feature,int selected_component,ViewCamera camera,float zoom);
void draw_live_generation_annotations(SDL_Renderer* renderer,const Game& game,ViewCamera camera,float zoom);
