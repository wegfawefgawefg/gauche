#include "../items/machine_fittings.hpp"
#include "boiler_render.hpp"
#include "boiler_tank.hpp"


#include <cmath>
#include <cstdio>

void draw_boiler_details(tr::Renderer* renderer, const GameGraphics& graphics,
                        const Entity& tank, SDL_FRect rect, LightColor light) {
    if (tank.kind != EntityKind::BoilerTank || tank.health <= 0) return;
    const Cell direction = tank.label_a == BoilerTell ? tank.point_b : outlet_direction(tank,tank.facing);
    tr::Texture* nozzle = texture_for(graphics,Sprite::BoilerNozzle);
    tr::texture_color(nozzle,light.red,light.green,light.blue);
    const double angle = std::atan2(static_cast<double>(direction.y),static_cast<double>(direction.x))*180.0/3.141592653589793;
    tr::draw_rotated(renderer,nozzle,nullptr,&rect,angle,nullptr,SDL_FLIP_NONE);
    tr::texture_color(nozzle,1,1,1);
    if (tank.ground_item.kind == ItemKind::PressureValve) {
        tr::Texture* valve = texture_for(graphics,Sprite::PressureValve);
        tr::texture_color(valve,light.red,light.green,light.blue);
        SDL_FRect wheel{rect.x+rect.w*.2F,rect.y+rect.h*.4F,rect.w*.45F,rect.h*.45F};
        tr::draw_texture(renderer,valve,nullptr,&wheel);
        tr::texture_color(valve,1,1,1);
    }
    // PRESSURE: A small always-visible gauge distinguishes ready, plugged and venting.
    const bool warning = tank.label_a == BoilerTell;
    const SDL_FColor color = warning ? SDL_FColor{.95F,.36F,.12F,1} :
        tank.timer_b > 0 ? SDL_FColor{.55F,.77F,.8F,1} : SDL_FColor{.78F,.66F,.33F,1};
    SDL_FRect bar{rect.x+2,rect.y+rect.h+1,rect.w-4,2};
    tr::set_color(renderer,.08F,.1F,.11F,1); tr::fill_rect(renderer,&bar);
    bar.w *= static_cast<float>(tank.counter_a)/100;
    tr::set_color(renderer,color.r,color.g,color.b,1); tr::fill_rect(renderer,&bar);
    if (tank.timer_b > 0 || warning || tank.counter_a > 0) {
        char label[24];
        if (tank.timer_b > 0) std::snprintf(label,sizeof(label),"PLUG %ds",(tank.timer_b+59)/60);
        else if (warning) std::snprintf(label,sizeof(label),"VENT %.1fs",static_cast<double>(tank.timer_a)/60);
        else std::snprintf(label,sizeof(label),"P %d",tank.counter_a);
        float sx=1, sy=1;
        tr::get_scale(renderer,&sx,&sy);
        tr::set_scale(renderer,sx*.5F,sy*.5F);
        tr::debug_text(renderer,rect.x*2,(rect.y+rect.h+4)*2,label);
        tr::set_scale(renderer,sx,sy);
    }
}
