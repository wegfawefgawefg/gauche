#include "generation_overlay.hpp"

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
    SDL_BlendMode old_blend;SDL_GetRenderDrawBlendMode(renderer,&old_blend);
    float red=0,green=0,blue=0,alpha=0;SDL_GetRenderDrawColorFloat(renderer,&red,&green,&blue,&alpha);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    if (selected_feature>=0) {
        const auto* decision=feature_decision(report,static_cast<GenerationFeature>(selected_feature));
        if (decision) for (const auto& region:decision->regions) {
            auto box=tile_rect(region.low,camera,zoom);
            box.w=static_cast<float>(region.high.x-region.low.x)*tile_pixels(zoom);
            box.h=static_cast<float>(region.high.y-region.low.y)*tile_pixels(zoom);
            SDL_SetRenderDrawColor(renderer,255,210,80,65);SDL_RenderFillRect(renderer,&box);
            SDL_SetRenderDrawColor(renderer,255,230,90,255);SDL_RenderRect(renderer,&box);
        }
    }
    if (selected_component>=0 && static_cast<std::size_t>(selected_component)<report.components.size()) {
        const auto& child=report.components[static_cast<std::size_t>(selected_component)];
        SDL_SetRenderDrawColor(renderer,child.placed ? 90 : 255,child.placed ? 255 : 100,210,255);
        const auto mark=[&](Cell cell) {
            const auto box=tile_rect(cell,camera,zoom);
            SDL_RenderLine(renderer,box.x,box.y,box.x+box.w,box.y+box.h);
            SDL_RenderLine(renderer,box.x+box.w,box.y,box.x,box.y+box.h);
        };
        if (child.cells.empty()) mark(child.anchor);
        else for (Cell cell:child.cells) {
            if (child.guide.empty()) mark(cell);
            else {
                SDL_SetRenderDrawColor(renderer,90,240,210,100);
                const auto box=tile_rect(cell,camera,zoom);SDL_RenderFillRect(renderer,&box);
            }
        }
        SDL_SetRenderDrawColor(renderer,255,140,90,190);
        for (Cell cell:child.rejected_cells) mark(cell);
        SDL_SetRenderDrawColor(renderer,child.placed ? 90 : 255,child.placed ? 255 : 100,210,255);
        for (std::size_t i=1;!child.guide.empty() && i<child.guide.size()+(child.guide_closed ? 1U : 0U);++i) {
            const auto a=tile_rect(child.guide[i-1],camera,zoom);
            const auto b=tile_rect(child.guide[i%child.guide.size()],camera,zoom);
            const float offset=child.guide_closed && !child.guide_cell_centers ? 0.0F : a.w*.5F;
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
