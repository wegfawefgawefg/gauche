#pragma once
#include "../render.hpp"
#include "../world/generation_report.hpp"
#include <memory>

struct GenerationAnnotations {
    int scope=0; // Selected child, selected feature's children, or all recorded children.
    bool bounds=true,guides=true,areas=true,sites=true,rejected=true,labels=false;
};
// Presentation only. Weak ownership avoids retaining a discarded floor report;
// selection indices never cross between preview checkpoints and live reports.
struct LiveGenerationInspector {
    std::weak_ptr<const GenerationReport> report;
    int selected_feature=-1,selected_component=-1;
    bool overlay=false;
    GenerationAnnotations annotations;
};
LiveGenerationInspector& live_generation_inspector();
LiveGenerationInspector& bind_live_generation_report(const Game& game);
void draw_generation_annotations(SDL_Renderer* renderer,const GenerationReport& report,
    int selected_feature,int selected_component,ViewCamera camera,float zoom,
    const GenerationAnnotations& options={});
void draw_generation_annotation_controls(GenerationAnnotations& options);
void draw_live_generation_annotations(SDL_Renderer* renderer,const Game& game,ViewCamera camera,float zoom);
