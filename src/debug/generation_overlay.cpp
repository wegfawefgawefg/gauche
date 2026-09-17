#include "generation_overlay.hpp"
#include "../lighting/render.hpp"
#include <algorithm>
#include <span>
#include <imgui.h>
#include <cstdio>

namespace {
void area(SDL_Renderer* renderer,std::span<const Cell> cells,ViewCamera camera,float zoom,SDL_Color color) {
    auto sorted=std::vector<Cell>(cells.begin(),cells.end());
    const auto less=[](Cell a,Cell b){return a.y==b.y ? a.x<b.x : a.y<b.y;};
    std::sort(sorted.begin(),sorted.end(),less);
    sorted.erase(std::unique(sorted.begin(),sorted.end()),sorted.end());
    const auto contains=[&](Cell c){return std::binary_search(sorted.begin(),sorted.end(),c,less);};
    const SDL_FColor fill{color.r/255.0F,color.g/255.0F,color.b/255.0F,.10F};
    for(Cell c:sorted)draw_flat_tile(renderer,nullptr,tile_rect(c,camera,zoom),fill);
    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,255);
    // Trace exposed edges, including holes and disconnected islands. Interior
    // cells have neither crosses nor grid lines to obscure the generated scene.
    for(Cell c:sorted) {
        const auto box=tile_rect(c,camera,zoom);
        if(!contains(c+Cell{-1,0}))SDL_RenderLine(renderer,box.x,box.y,box.x,box.y+box.h);
        if(!contains(c+Cell{1,0}))SDL_RenderLine(renderer,box.x+box.w,box.y,box.x+box.w,box.y+box.h);
        if(!contains(c+Cell{0,-1}))SDL_RenderLine(renderer,box.x,box.y,box.x+box.w,box.y);
        if(!contains(c+Cell{0,1}))SDL_RenderLine(renderer,box.x,box.y+box.h,box.x+box.w,box.y+box.h);
    }
}
}

LiveGenerationInspector& live_generation_inspector() {
    static LiveGenerationInspector state;
    return state;
}
LiveGenerationInspector& bind_live_generation_report(const Game& game) {
    auto& state=live_generation_inspector();
    if(!game.generation_report || state.report.lock()!=game.generation_report) {
        state.report=game.generation_report;
        state.selected_feature=state.selected_component=-1;
    }
    return state;
}
void draw_generation_annotations(SDL_Renderer* renderer,const GenerationReport& report,
    int selected_feature,int selected_component,ViewCamera camera,float zoom,const GenerationAnnotations& options) {
    if(report.geometry_omitted)return;
    SDL_BlendMode old_blend;SDL_GetRenderDrawBlendMode(renderer,&old_blend);
    float red=0,green=0,blue=0,alpha=0;SDL_GetRenderDrawColorFloat(renderer,&red,&green,&blue,&alpha);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    const ComponentDecision* child=selected_component>=0 && static_cast<std::size_t>(selected_component)<report.components.size() ?
        &report.components[static_cast<std::size_t>(selected_component)] : nullptr;
    if (options.bounds && selected_feature>=0) {
        const auto* decision=feature_decision(report,static_cast<GenerationFeature>(selected_feature));
        if (decision) for (const auto& region:decision->regions) {
            auto box=tile_rect(region.low,camera,zoom);
            box.w=static_cast<float>(region.high.x-region.low.x)*tile_pixels(zoom);
            box.h=static_cast<float>(region.high.y-region.low.y)*tile_pixels(zoom);
            const bool focused=child && static_cast<int>(child->feature)==selected_feature;
            if(!focused)draw_flat_tile(renderer,nullptr,box,{1,.82F,.31F,.10F});
            SDL_SetRenderDrawColor(renderer,255,230,90,focused ? 110 : 255);SDL_RenderRect(renderer,&box);
        }
    }
    struct Label {SDL_FRect box;SDL_FPoint anchor;std::string text,counts;};
    std::vector<Label> labels;
    // Draw the focused child first so its label wins when nearby labels overlap.
    for(std::size_t n=0;n<report.components.size();++n) {
        const std::size_t index=child ? (n==0 ? static_cast<std::size_t>(selected_component) :
            n<=static_cast<std::size_t>(selected_component) ? n-1 : n) : n;
        const auto& selected=report.components[index];
        if(options.scope==0 && &selected!=child)continue;
        if(options.scope==1 && static_cast<int>(selected.feature)!=selected_feature)continue;
        SDL_SetRenderDrawColor(renderer,selected.placed ? 90 : 255,selected.placed ? 255 : 100,210,255);
        const auto mark=[&](Cell cell) {
            const auto box=tile_rect(cell,camera,zoom);
            SDL_RenderLine(renderer,box.x,box.y,box.x+box.w,box.y+box.h);
            SDL_RenderLine(renderer,box.x+box.w,box.y,box.x,box.y+box.h);
        };
        if (selected.cells.empty()) {if(options.sites)mark(selected.anchor);}
        else if(selected.cells_are_area) {if(options.areas)area(renderer,selected.cells,camera,zoom,{90,240,210,255});}
        else if(options.sites)for(Cell cell:selected.cells)mark(cell);
        SDL_SetRenderDrawColor(renderer,255,140,90,190);
        if(options.rejected) {
            if(selected.cells_are_area)area(renderer,selected.rejected_cells,camera,zoom,{255,140,90,255});
            else for (Cell cell:selected.rejected_cells) mark(cell);
        }
        SDL_SetRenderDrawColor(renderer,selected.placed ? 90 : 255,selected.placed ? 255 : 100,210,255);
        for (std::size_t i=1;options.guides && !selected.guide.empty() && i<selected.guide.size()+(selected.guide_closed ? 1U : 0U);++i) {
            const auto a=tile_rect(selected.guide[i-1],camera,zoom);
            const auto b=tile_rect(selected.guide[i%selected.guide.size()],camera,zoom);
            const float offset=selected.guide_closed && !selected.guide_cell_centers ? 0.0F : a.w*.5F;
            SDL_RenderLine(renderer,a.x+offset,a.y+offset,b.x+offset,b.y+offset);
        }
        const bool marked=(options.guides && !selected.guide.empty()) ||
            (options.areas && selected.cells_are_area && !selected.cells.empty()) ||
            (options.sites && !selected.cells_are_area) || (options.rejected && !selected.rejected_cells.empty());
        if(options.labels && (marked || &selected==child) && labels.size()<64) {
            const auto anchor=tile_rect(selected.anchor,camera,zoom);
            if(anchor.x<0 || anchor.x>620 || anchor.y<24 || anchor.y>312)continue;
            std::string text="#"+std::to_string(index)+" "+selected.slot+": "+selected.choice;
            if(text.size()>48)text=text.substr(0,45)+"...";
            char counts[80];std::snprintf(counts,sizeof(counts),"%d %s / %zu skipped",selected.placed,
                selected.cells_are_area ? "tiles" : "sites",selected.rejected_cells.size());
            const float width=static_cast<float>(std::max(text.size(),std::char_traits<char>::length(counts)))*4+4;
            SDL_FRect box{std::clamp(anchor.x,2.0F,638-width),anchor.y,width,13};
            bool clear=false;
            for(int attempt=0;attempt<4 && box.y+box.h<320;++attempt,box.y+=15) {
                if(std::none_of(labels.begin(),labels.end(),[&](const Label& other){return SDL_HasRectIntersectionFloat(&box,&other.box);})) {clear=true;break;}
            }
            if(!clear)continue;
            labels.push_back({box,{anchor.x+anchor.w*.5F,anchor.y+anchor.h*.5F},std::move(text),counts});
        }
    }
    // Labels are a final layer so later component guides cannot strike through text.
    for(const auto& label:labels) {
        const auto& box=label.box;
        SDL_SetRenderDrawColor(renderer,150,210,190,190);
        SDL_RenderLine(renderer,label.anchor.x,label.anchor.y,box.x,box.y+box.h*.5F);
    }
    float sx=1,sy=1;SDL_GetRenderScale(renderer,&sx,&sy);
    for(const auto& label:labels) {
        const auto& box=label.box;
        SDL_SetRenderDrawColor(renderer,10,16,20,245);SDL_RenderFillRect(renderer,&box);
        SDL_SetRenderScale(renderer,sx*.5F,sy*.5F);
        SDL_SetRenderDrawColor(renderer,205,250,235,255);SDL_RenderDebugText(renderer,(box.x+2)*2,(box.y+2)*2,label.text.c_str());
        SDL_RenderDebugText(renderer,(box.x+2)*2,(box.y+7)*2,label.counts.c_str());
        SDL_SetRenderScale(renderer,sx,sy);
    }
    SDL_SetRenderDrawColorFloat(renderer,red,green,blue,alpha);
    SDL_SetRenderDrawBlendMode(renderer,old_blend);
}
void draw_live_generation_annotations(SDL_Renderer* renderer,const Game& game,ViewCamera camera,float zoom) {
    if(!GAUCHE_DEV_MODE)return;
    auto& state=bind_live_generation_report(game);
    if(!state.overlay || !game.generation_report)return;
    draw_generation_annotations(renderer,*game.generation_report,state.selected_feature,state.selected_component,camera,zoom,state.annotations);
}

void draw_generation_annotation_controls(GenerationAnnotations& options) {
    ImGui::Combo("Annotation scope",&options.scope,"Selected child\0Selected feature\0All recorded components\0");
    ImGui::Checkbox("Selected feature bounds",&options.bounds);
    ImGui::Checkbox("Carving lines / polygons",&options.guides);
    ImGui::Checkbox("Affected terrain areas",&options.areas);
    ImGui::Checkbox("Placement / empty sites",&options.sites);
    ImGui::Checkbox("Rejected cells",&options.rejected);
    ImGui::Checkbox("Component labels / counts",&options.labels);
    ImGui::TextWrapped("Recorded guides describe proposals; affected cells show what was applied. Labels use child row numbers, skip overlaps and favor the selected child. Select a row in Rolls for full names, weights and reasons. Uninstrumented operations have no guides.");
}
