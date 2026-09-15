#include "boiler_render.hpp"
#include "boiler_tank.hpp"


#include <cmath>
#include <cstdio>

void draw_boiler_details(SDL_Renderer* renderer, const GameGraphics& graphics,
                        const Entity& tank, SDL_FRect rect, LightColor light) {
    if (tank.kind != EntityKind::BoilerTank || tank.health <= 0) return;
    const Cell direction = tank.label_a == BoilerTell ? tank.point_b : tank.facing;
    SDL_Texture* nozzle = texture_for(graphics,Sprite::BoilerNozzle);
    SDL_SetTextureColorModFloat(nozzle,light.red,light.green,light.blue);
    const double angle = std::atan2(static_cast<double>(direction.y),static_cast<double>(direction.x))*180.0/3.141592653589793;
    SDL_RenderTextureRotated(renderer,nozzle,nullptr,&rect,angle,nullptr,SDL_FLIP_NONE);
    SDL_SetTextureColorModFloat(nozzle,1,1,1);
    if (tank.ground_item.kind == ItemKind::PressureValve) {
        SDL_Texture* valve = texture_for(graphics,Sprite::PressureValve);
        SDL_SetTextureColorModFloat(valve,light.red,light.green,light.blue);
        SDL_FRect wheel{rect.x+rect.w*.2F,rect.y+rect.h*.4F,rect.w*.45F,rect.h*.45F};
        SDL_RenderTexture(renderer,valve,nullptr,&wheel);
        SDL_SetTextureColorModFloat(valve,1,1,1);
    }
    // PRESSURE: A small always-visible gauge distinguishes ready, plugged and venting.
    const bool warning = tank.label_a == BoilerTell;
    const SDL_FColor color = warning ? SDL_FColor{.95F,.36F,.12F,1} :
        tank.timer_b > 0 ? SDL_FColor{.55F,.77F,.8F,1} : SDL_FColor{.78F,.66F,.33F,1};
    SDL_FRect bar{rect.x+2,rect.y+rect.h+1,rect.w-4,2};
    SDL_SetRenderDrawColorFloat(renderer,.08F,.1F,.11F,1); SDL_RenderFillRect(renderer,&bar);
    bar.w *= static_cast<float>(tank.counter_a)/100;
    SDL_SetRenderDrawColorFloat(renderer,color.r,color.g,color.b,1); SDL_RenderFillRect(renderer,&bar);
    if (tank.timer_b > 0 || warning || tank.counter_a > 0) {
        char label[24];
        if (tank.timer_b > 0) std::snprintf(label,sizeof(label),"PLUG %ds",(tank.timer_b+59)/60);
        else if (warning) std::snprintf(label,sizeof(label),"VENT %.1fs",static_cast<double>(tank.timer_a)/60);
        else std::snprintf(label,sizeof(label),"P %d",tank.counter_a);
        float sx=1, sy=1;
        SDL_GetRenderScale(renderer,&sx,&sy);
        SDL_SetRenderScale(renderer,sx*.5F,sy*.5F);
        SDL_RenderDebugText(renderer,rect.x*2,(rect.y+rect.h+4)*2,label);
        SDL_SetRenderScale(renderer,sx,sy);
    }
}
