#include "../debug/performance.hpp"
#include "ambient_inspector.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>

void draw_ambient_annotations(SDL_Renderer* renderer,const Game& game,ViewCamera camera,float zoom) {
    PerfScope perf_scope(PerfZone::Annotations);
    const auto& state=ambient_inspector();
    if(!GAUCHE_DEV_MODE || !state.overlay || state.game!=&game || !state.audio)return;
    SDL_BlendMode blend;SDL_GetRenderDrawBlendMode(renderer,&blend);
    float r=0,g=0,b=0,a=0;SDL_GetRenderDrawColorFloat(renderer,&r,&g,&b,&a);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    std::vector<SDL_FRect> labels;
    for(std::size_t i=0;i<state.audio->sources.size();++i) {
        if(state.selected_only && static_cast<int>(i)!=state.selected)continue;
        const auto& source=state.audio->sources[i];const auto& spec=ambient_specs[static_cast<std::size_t>(source.cue)];
        if(state.mode && static_cast<int>(spec.mode)!=state.mode-1)continue;
        const auto tile=tile_rect(ambient_source_cell(source,game),camera,zoom);
        const float x=tile.x+tile.w*.5F,y=tile.y+tile.h*.5F;
        const auto circle=[&](float radius) {
            const float pixels=radius*tile_pixels(zoom);
            SDL_FPoint points[49];for(int n=0;n<=48;++n) {
                const float angle=static_cast<float>(n)*6.2831853F/48;
                points[n]={x+std::cos(angle)*pixels,y+std::sin(angle)*pixels};
            }SDL_RenderLines(renderer,points,49);
        };
        SDL_SetRenderDrawColor(renderer,90,225,230,180);
        SDL_RenderLine(renderer,x-3,y,x+3,y);SDL_RenderLine(renderer,x,y-3,x,y+3);
        if(state.ranges && !source.global){circle(spec.near_radius);circle(spec.far_radius);}
        if(state.triggers && spec.mode==AmbientMode::Enter) {
            SDL_SetRenderDrawColor(renderer,source.consumed&&!state.preview ? 155 : 255,155,80,230);
            const float pixels=spec.trigger_radius*tile_pixels(zoom);
            const SDL_FPoint points[]{{x,y-pixels},{x+pixels,y},{x,y+pixels},{x-pixels,y},{x,y-pixels}};
            SDL_RenderLines(renderer,points,5);
        }
        if(!state.labels || x<0 || x>620 || y<24 || y>308 || labels.size()>=48)continue;
        char label[100];std::snprintf(label,sizeof(label),"#%zu %s%s%s",i,spec.name,source.global ? " [global]" : "",
            state.preview ? " [preview]" : source.pending ? " [pending]" : source.consumed ? " [fired]" : spec.mode==AmbientMode::Enter ? " [armed]" : "");
        const float width=static_cast<float>(std::char_traits<char>::length(label))*8+4;
        SDL_FRect box{std::clamp(x+4,2.0F,638-width),y,width,12};
        bool clear=false;for(int n=0;n<8 && box.y+12<320;++n,box.y+=13) {
            if(std::none_of(labels.begin(),labels.end(),[&](const SDL_FRect& other){return SDL_HasRectIntersectionFloat(&box,&other);})) {clear=true;break;}
        }
        if(clear){labels.push_back(box);SDL_SetRenderDrawColor(renderer,10,16,20,225);SDL_RenderFillRect(renderer,&box);
            SDL_SetRenderDrawColor(renderer,230,235,190,255);SDL_RenderDebugText(renderer,box.x+2,box.y+2,label);}
    }
    SDL_SetRenderDrawColorFloat(renderer,r,g,b,a);SDL_SetRenderDrawBlendMode(renderer,blend);
}
