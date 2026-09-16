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
    // An outlet is derived from real terrain/current state and survives snapshots.
    // Surface rendering follows terrain, so falling streaks can extend into a pit.
    if (flow!=Cell{}) {
        const auto& next=stage.at_or_border(cell+flow);
        const bool drop=next.kind==TileKind::Chasm;
        const bool edge=next.kind==TileKind::Wall && next.break_rule==BreakRule::Unbreakable;
        if (drop || edge) {
            const float cx=rect.x+rect.w*(.5F+.35F*static_cast<float>(flow.x));
            const float cy=rect.y+rect.h*(.5F+.35F*static_cast<float>(flow.y));
            const SDL_FRect mouth{cx-rect.w*.18F,cy-rect.h*.18F,rect.w*.36F,rect.h*.36F};
            SDL_SetRenderDrawColorFloat(renderer,light.red*.05F,light.green*.09F,light.blue*.1F,1);
            SDL_RenderFillRect(renderer,&mouth);
            if (drop) for (int i=0;i<4;++i) {
                const float fall=static_cast<float>((tick+static_cast<std::uint64_t>(i*15))%60)/60.0F;
                const float across=(static_cast<float>(i)-1.5F)*.08F;
                const float x=rect.x+rect.w*(.5F+static_cast<float>(flow.x)*(.55F+fall*.4F)-static_cast<float>(flow.y)*across);
                const float y=rect.y+rect.h*(.5F+static_cast<float>(flow.y)*(.55F+fall*.4F)+static_cast<float>(flow.x)*across+fall*.25F);
                SDL_SetRenderDrawColorFloat(renderer,light.red*.5F,light.green*.75F,light.blue*.9F,(1.0F-fall)*.8F);
                SDL_RenderLine(renderer,x,y,x,y+rect.h*(.08F+fall*.12F));
            }
            for (int i=0;i<3;++i) {
                const float phase=static_cast<float>((tick+static_cast<std::uint64_t>(i*20))%60)/60.0F;
                const float across=(static_cast<float>(i)-1)*.1F;
                const float x=cx+rect.w*(-static_cast<float>(flow.y)*across+static_cast<float>(flow.x)*(phase-.5F)*.25F);
                const float y=cy+rect.h*(static_cast<float>(flow.x)*across+static_cast<float>(flow.y)*(phase-.5F)*.25F);
                SDL_SetRenderDrawColorFloat(renderer,light.red*.65F,light.green*.8F,light.blue*.9F,drop ? 1.0F-phase*.6F : .6F);
                SDL_RenderLine(renderer,x,y,x+static_cast<float>(flow.x)*rect.w*.12F,y+static_cast<float>(flow.y)*rect.h*.12F);
            }
        }
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
