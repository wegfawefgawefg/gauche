#include "choir_render.hpp"
#include "tar_choir.hpp"
#include <cmath>

void apply_choir_pose(const Entity& actor,std::uint64_t tick,SDL_FRect& rect,double& angle) {
    if (actor.kind!=EntityKind::TarChoir || actor.health<=0) return;
    const bool warning=actor.label_a==ChoirHum;
    const float beat=std::sin(static_cast<float>(tick%6000)*.09F);
    angle+=beat*(warning ? 7 : 2);
    if (warning) {
        const float fill=1-static_cast<float>(actor.timer_a)/static_cast<float>(48+actor.counter_a*12);
        const float width=1+.18F*fill,height=1+.16F*fill;
        rect.x-=rect.w*(width-1)*.5F;rect.y-=rect.h*(height-1);rect.w*=width;rect.h*=height;
    } else if (actor.label_a==ChoirCough) {
        rect.y+=rect.h*.18F;rect.h*=.82F;rect.x-=rect.w*.08F;rect.w*=1.16F;
    }
}
