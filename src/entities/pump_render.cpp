#include "../items/machine_fittings.hpp"
#include "pump_render.hpp"
#include "emergency_pump.hpp"
#include <algorithm>

namespace {
LightColor liquid_color(LiquidKind kind) {
    switch (kind) {
    case LiquidKind::Tar: return {.42F,.29F,.21F};
    case LiquidKind::Oil: return {.68F,.48F,.20F};
    case LiquidKind::Sap: case LiquidKind::Honey: return {.94F,.67F,.23F};
    case LiquidKind::Rot: return {.61F,.76F,.28F};
    case LiquidKind::Coolant: return {.31F,.94F,.75F};
    case LiquidKind::Brine: return {.81F,.89F,.86F};
    default: return {.44F,.72F,.94F};
    }
}
}
void apply_pump_pose(const Entity& actor,SDL_FRect& rect) {
    if (actor.kind==EntityKind::EmergencyPump && (actor.label_a==PumpWarn || actor.label_a==PumpFill))
        rect.y+=actor.timer_a/3%2==0 ? -rect.h*.025F : rect.h*.025F;
}
void draw_pump_nozzle(tr::Renderer* renderer,const Entity& actor,SDL_FRect rect,LightColor light) {
    if (actor.kind!=EntityKind::EmergencyPump || actor.health<=0) return;
    const auto color=liquid_color(static_cast<LiquidKind>(actor.counter_a));
    const float size=rect.w,cx=rect.x+size*.5F,cy=rect.y+rect.h*.5F;
    Cell direction=outlet_direction(actor,actor.facing);
    const bool jet=actor.label_a==PumpRecover && actor.label_b>0 && actor.timer_a>60 && actor.cell==actor.point_a;
    if (actor.label_a==PumpWarn || jet) direction=actor.point_b;
    const float dx=static_cast<float>(direction.x),dy=static_cast<float>(direction.y);
    tr::set_color(renderer,light.red*.7F,light.green*.65F,light.blue*.5F,1);
    tr::line(renderer,cx+dx*size*.2F,cy+dy*size*.2F,cx+dx*size*.6F,cy+dy*size*.6F);
    // A small tank stripe shows whether this is a water or fuel load even at rest.
    tr::set_color(renderer,light.red*color.red,light.green*color.green,light.blue*color.blue,.95F);
    SDL_FRect gauge{rect.x+size*.3F,rect.y+size*.65F,size*.34F*static_cast<float>(actor.counter_b)/1800,size*.06F};
    tr::fill_rect(renderer,&gauge);
    if (actor.label_a==PumpWarn) {
        const float progress=1-static_cast<float>(actor.timer_a)/36;
        const float extent=size*(.08F+.12F*progress);
        SDL_FRect tip{cx+dx*size*.6F-extent*.5F,cy+dy*size*.6F-extent*.5F,extent,extent};
        tr::rect(renderer,&tip);
        tr::line(renderer,tip.x,tip.y,tip.x+extent*progress,tip.y+extent);
    }
    if (actor.label_a==PumpFill) {
        const Cell offset=actor.point_b-actor.cell;
        const float pull=static_cast<float>(actor.timer_a%12)/12;
        SDL_FRect drop{cx+static_cast<float>(offset.x)*size*pull-size*.04F,
            cy+static_cast<float>(offset.y)*size*pull-size*.04F,size*.08F,size*.08F};
        tr::fill_rect(renderer,&drop);
    }
    if (!jet) return;
    const float fade=static_cast<float>(actor.timer_a-60)/12;
    tr::set_color(renderer,light.red*color.red,light.green*color.green,light.blue*color.blue,fade*.9F);
    for (int lane=-1;lane<=1;++lane) {
        const float offset=static_cast<float>(lane)*size*.06F;
        tr::line(renderer,cx+dx*size*.6F-dy*offset,cy+dy*size*.6F+dx*offset,
            cx+dx*size*static_cast<float>(actor.label_b)-dy*offset*2,
            cy+dy*size*static_cast<float>(actor.label_b)+dx*offset*2);
    }
}
