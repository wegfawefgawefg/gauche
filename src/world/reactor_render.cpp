#include "reactor_render.hpp"
#include "../ui/scale.hpp"
#include "../ui/text.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>

void draw_reactor_hazards(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,ViewCamera camera,float zoom) {
    const Entity* core=reactor_core(game);
    if (!core || !core->fixture_open) return;
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    for (int i=core->counter_b;i<core->counter_a;++i) {
        const ReactorEvent& event=game.reactor_front[static_cast<std::size_t>(i)];
        const Cell cell{static_cast<int>(event.tile)%game.stage.width,static_cast<int>(event.tile)/game.stage.width};
        if (!exposed_reactor_cell(game.stage,cell)) continue;
        const SDL_FRect r=tile_rect(cell,camera,zoom);
        if (r.x+r.w<0 || r.x>640 || r.y+r.h<0 || r.y>360) continue;
        const float pulse=((core->counter_c+cell.x*13+cell.y*7)/12)%2 ? .48F : .2F;
        SDL_SetRenderDrawColorFloat(renderer,.69F,.77F,.27F,pulse);
        // Small pulsing broken seam, not a solid debug-style attack square.
        SDL_RenderLine(renderer,r.x+r.w*.15F,r.y+r.h*.72F,r.x+r.w*.46F,r.y+r.h*.5F);
        SDL_RenderLine(renderer,r.x+r.w*.46F,r.y+r.h*.5F,r.x+r.w*.39F,r.y+r.h*.28F);
        SDL_RenderLine(renderer,r.x+r.w*.39F,r.y+r.h*.28F,r.x+r.w*.8F,r.y+r.h*.14F);
    }
    const int rx=static_cast<int>(320/tile_pixels(zoom))+2,ry=static_cast<int>(180/tile_pixels(zoom))+2;
    SDL_Texture* texture=texture_for(graphics,Sprite::ReactorFlame);
    for (int y=std::max(0,static_cast<int>(camera.y)-ry);y<std::min(game.stage.height,static_cast<int>(camera.y)+ry+1);++y)
        for (int x=std::max(0,static_cast<int>(camera.x)-rx);x<std::min(game.stage.width,static_cast<int>(camera.x)+rx+1);++x) {
            const Surface& s=game.stage.at({x,y})->surface;
            if (!s.reactor_fire || !s.fire_ticks || !exposed_reactor_cell(game.stage,{x,y})) continue;
            const SDL_FRect r=tile_rect({x,y},camera,zoom);
            if (r.x+r.w<0 || r.x>640 || r.y+r.h<0 || r.y>360) continue;
            // Stable per-cell offsets and unequal tongues break the grid.
            const unsigned seed=static_cast<unsigned>(x)*73856093U ^ static_cast<unsigned>(y)*19349663U;
            const float flicker=static_cast<float>((game.tick/9+seed)%5)*.025F;
            const float height=(.52F+static_cast<float>((seed>>8)%5)*.06F+flicker)*r.h;
            SDL_FRect flame{r.x+r.w*(.07F+static_cast<float>(seed%7)*.045F),r.y+r.h*.82F-height,r.w*.68F,height};
            SDL_SetTextureColorModFloat(texture,.82F,.9F,.65F);
            SDL_SetTextureAlphaModFloat(texture,.9F);
            SDL_RenderTextureRotated(renderer,texture,nullptr,&flame,0,nullptr,seed%2 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
            SDL_SetTextureColorModFloat(texture,1,1,1);SDL_SetTextureAlphaModFloat(texture,1);
        }
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
}

void draw_reactor_status(SDL_Renderer* renderer,const Game& game) {
    const Entity* core=reactor_core(game);
    if (!core || (game.run.phase!=RunPhase::Playing && !game.game_over)) return;
    const HudScale scale{renderer};
    const int left=std::max(0,reactor_deadline-core->counter_c);
    char text[96];
    if (!core->fixture_open) std::snprintf(text,sizeof(text),"REACTOR SHUTDOWN OPENS EXIT. THEN 60S TO ESCAPE.");
    else if (!left) std::snprintf(text,sizeof(text),"REACTOR LOST");
    else std::snprintf(text,sizeof(text),"MELTDOWN %d:%02d  -  EXIT OPEN / LEAVE TOGETHER",(left+59)/3600,((left+59)/60)%60);
    const float width=static_cast<float>(std::strlen(text))*4+16;
    const float x=(640/ui_scale-width)*.5F,y=322/ui_scale;
    const auto banner=[&](float offset,SDL_FColor color) {
        SDL_Vertex vertices[]{{{x+offset+3,y+offset},color,{}},{{x+width+offset,y+offset},color,{}},
            {{x+width+offset-3,y+offset+18},color,{}},{{x+offset,y+offset+18},color,{}}};
        constexpr int indices[]{0,1,2,0,2,3};SDL_RenderGeometry(renderer,nullptr,vertices,4,indices,6);
    };
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    banner(3,{0,0,0,.7F});
    banner(0,core->fixture_open && left<=600 ? SDL_FColor{.37F,.08F,.04F,.96F} : SDL_FColor{.12F,.16F,.065F,.94F});
    small_ui_text(renderer,x+8,y+6,text,224,223,157);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
}
