#include "lava_eruption_render.hpp"
#include <cmath>

void draw_lava_eruptions(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                         ViewCamera camera,float zoom,bool airborne) {
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    for (const LavaVent& vent:game.lava_vents) {
        if (vent.phase==LavaPhase::Cooling) continue;
        if (vent.phase!=LavaPhase::Airborne && game.stage.at_or_border(vent.source).kind!=TileKind::Lava) continue;
        const SDL_FRect origin=tile_rect(vent.source,camera,zoom),target=tile_rect(vent.target,camera,zoom);
        if ((origin.x<-128 && target.x<-128) || (origin.x>768 && target.x>768) ||
            (origin.y<-128 && target.y<-128) || (origin.y>488 && target.y>488)) continue;
        if (!airborne) {
            const float pulse=.55F+static_cast<float>((game.tick/8)%2)*.3F;
            SDL_SetRenderDrawColorFloat(renderer,.95F,.47F,.10F,pulse);
            // Four inward chevrons mark one cell; empty centre keeps occupants readable.
            const float cx=target.x+target.w*.5F,cy=target.y+target.h*.5F;
            for (int side:{-1,1}) {
                const float s=static_cast<float>(side);
                SDL_RenderLine(renderer,cx+s*target.w*.4F,cy-target.h*.15F,cx+s*target.w*.3F,cy);
                SDL_RenderLine(renderer,cx+s*target.w*.3F,cy,cx+s*target.w*.4F,cy+target.h*.15F);
                SDL_RenderLine(renderer,cx-target.w*.15F,cy+s*target.h*.4F,cx,cy+s*target.h*.3F);
                SDL_RenderLine(renderer,cx,cy+s*target.h*.3F,cx+target.w*.15F,cy+s*target.h*.4F);
            }
            if (vent.phase==LavaPhase::Swelling) {
                const float size=.25F+.45F*(1-static_cast<float>(vent.ticks)/lava_warning_ticks);
                SDL_FRect dome{origin.x+origin.w*(1-size)*.5F,origin.y+origin.h*(.6F-size),origin.w*size,origin.h*size};
                SDL_RenderTexture(renderer,texture_for(graphics,Sprite::LavaGlob),nullptr,&dome);
            }
            continue;
        }
        if (vent.phase!=LavaPhase::Airborne) continue;
        const float t=1-static_cast<float>(vent.ticks)/lava_flight_ticks;
        const float x=origin.x+(target.x-origin.x)*t+origin.w*.5F;
        const float y=origin.y+(target.y-origin.y)*t+origin.h*.65F;
        const float height=std::sin(t*3.14159265F)*origin.h*1.4F;
        SDL_SetRenderDrawColorFloat(renderer,.02F,.01F,0,.65F);
        const SDL_FRect shadow{x-origin.w*.18F,y,origin.w*.36F,origin.h*.1F};
        SDL_RenderFillRect(renderer,&shadow);
        const SDL_FRect glob{x-origin.w*.3F,y-height-origin.h*.45F,origin.w*.6F,origin.h*.6F};
        SDL_RenderTextureRotated(renderer,texture_for(graphics,Sprite::LavaGlob),nullptr,&glob,t*160,nullptr,SDL_FLIP_NONE);
    }
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
}
