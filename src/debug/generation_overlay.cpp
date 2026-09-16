#include "generation_overlay.hpp"
#include "../lighting/render.hpp"
#include <algorithm>
#include <span>

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
    int selected_feature,int selected_component,ViewCamera camera,float zoom) {
    if(report.geometry_omitted)return;
    SDL_BlendMode old_blend;SDL_GetRenderDrawBlendMode(renderer,&old_blend);
    float red=0,green=0,blue=0,alpha=0;SDL_GetRenderDrawColorFloat(renderer,&red,&green,&blue,&alpha);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    const ComponentDecision* child=selected_component>=0 && static_cast<std::size_t>(selected_component)<report.components.size() ?
        &report.components[static_cast<std::size_t>(selected_component)] : nullptr;
    if (selected_feature>=0) {
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
    if (child) {
        const auto& selected=*child;
        SDL_SetRenderDrawColor(renderer,selected.placed ? 90 : 255,selected.placed ? 255 : 100,210,255);
        const auto mark=[&](Cell cell) {
            const auto box=tile_rect(cell,camera,zoom);
            SDL_RenderLine(renderer,box.x,box.y,box.x+box.w,box.y+box.h);
            SDL_RenderLine(renderer,box.x+box.w,box.y,box.x,box.y+box.h);
        };
        if (selected.cells.empty()) mark(selected.anchor);
        else if(selected.cells_are_area)area(renderer,selected.cells,camera,zoom,{90,240,210,255});
        else for(Cell cell:selected.cells)mark(cell);
        SDL_SetRenderDrawColor(renderer,255,140,90,190);
        if(selected.cells_are_area)area(renderer,selected.rejected_cells,camera,zoom,{255,140,90,255});
        else for (Cell cell:selected.rejected_cells) mark(cell);
        SDL_SetRenderDrawColor(renderer,selected.placed ? 90 : 255,selected.placed ? 255 : 100,210,255);
        for (std::size_t i=1;!selected.guide.empty() && i<selected.guide.size()+(selected.guide_closed ? 1U : 0U);++i) {
            const auto a=tile_rect(selected.guide[i-1],camera,zoom);
            const auto b=tile_rect(selected.guide[i%selected.guide.size()],camera,zoom);
            const float offset=selected.guide_closed && !selected.guide_cell_centers ? 0.0F : a.w*.5F;
            SDL_RenderLine(renderer,a.x+offset,a.y+offset,b.x+offset,b.y+offset);
        }
    }
    SDL_SetRenderDrawColorFloat(renderer,red,green,blue,alpha);
    SDL_SetRenderDrawBlendMode(renderer,old_blend);
}
void draw_live_generation_annotations(SDL_Renderer* renderer,const Game& game,ViewCamera camera,float zoom) {
    if(!GAUCHE_DEV_MODE)return;
    auto& state=bind_live_generation_report(game);
    if(!state.overlay || !game.generation_report)return;
    draw_generation_annotations(renderer,*game.generation_report,state.selected_feature,state.selected_component,camera,zoom);
}
