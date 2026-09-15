#pragma once
#include "currents.hpp"
#include "../lighting/field.hpp"
#include <algorithm>

inline void draw_current_marks(SDL_Renderer* renderer,const Stage& stage,Cell cell,
    SDL_FRect rect,LightColor light,std::uint64_t tick) {
    const Tile& tile=stage.at_or_border(cell);
    const Cell flow=water_current(tile);
    if (flow!=Cell{}) {
        const float phase=static_cast<float>((tick+static_cast<std::uint64_t>((cell.x*7+cell.y*13)%60))%60)/60;
        const float x=rect.x+rect.w*(.5F+static_cast<float>(flow.x)*(phase-.5F)*.6F);
        const float y=rect.y+rect.h*(.5F+static_cast<float>(flow.y)*(phase-.5F)*.6F);
        SDL_SetRenderDrawColorFloat(renderer,light.red*.5F,light.green*.7F,light.blue*.75F,.45F);
        SDL_RenderLine(renderer,x-rect.w*.09F,y,x+rect.w*.09F,y);
    }
    if (tile.surface.still_ticks==0) return;
    const float alpha=.55F*std::min(1.0F,static_cast<float>(tile.surface.still_ticks)/30);
    SDL_SetRenderDrawColorFloat(renderer,light.red*.5F,light.green*.8F,light.blue*.8F,alpha);
    if (stage.at_or_border(cell+Cell{0,-1}).surface.still_ticks==0)
        SDL_RenderLine(renderer,rect.x+rect.w*.2F,rect.y,rect.x+rect.w*.8F,rect.y);
    if (stage.at_or_border(cell+Cell{0,1}).surface.still_ticks==0)
        SDL_RenderLine(renderer,rect.x+rect.w*.2F,rect.y+rect.h,rect.x+rect.w*.8F,rect.y+rect.h);
    if (stage.at_or_border(cell+Cell{-1,0}).surface.still_ticks==0)
        SDL_RenderLine(renderer,rect.x,rect.y+rect.h*.2F,rect.x,rect.y+rect.h*.8F);
    if (stage.at_or_border(cell+Cell{1,0}).surface.still_ticks==0)
        SDL_RenderLine(renderer,rect.x+rect.w,rect.y+rect.h*.2F,rect.x+rect.w,rect.y+rect.h*.8F);
}
