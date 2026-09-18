#include "ant_hauling_render.hpp"
#include "ant_hauling.hpp"

void draw_ant_ropes(tr::Renderer* renderer,const Game& game,const Entity& load,ViewCamera camera,float zoom,const LightingCache& lighting) {
    if (load.kind!=EntityKind::AntLoad || load.health<=0) return;
    const SDL_FRect body=tile_rect(load.cell,camera,zoom);
    const auto light=light_at_cell(lighting,load.cell);
    const Handle owner{static_cast<int>(&load-game.entities.data()),load.generation};
    for (const auto& ant:game.entities) {
        if (ant.kind!=EntityKind::Ant || ant.counter_a!=AntPuller || ant.entity_a!=owner || ant.health<=0 || distance(ant.cell,load.cell)>4 || ant.label_a!=AntWorking) continue;
        const SDL_FRect end=tile_rect(ant.cell,camera,zoom);
        const float x=body.x+body.w*.5F,y=body.y+body.h*.5F;
        const float ex=end.x+end.w*.5F,ey=end.y+end.h*.5F;
        const float slack=load.label_a==LoadWarn ? 0 : body.h*.2F;
        tr::set_color(renderer,light.red*.74F,light.green*.64F,light.blue*.42F,1);
        tr::line(renderer,x,y,(x+ex)*.5F,(y+ey)*.5F+slack);
        tr::line(renderer,(x+ex)*.5F,(y+ey)*.5F+slack,ex,ey);
    }
    if (load.label_a==LoadWarn) {
        SDL_FRect target=tile_rect(load.point_b,camera,zoom);
        const float inset=target.w*.12F;target.x+=inset;target.y+=inset;target.w-=inset*2;target.h-=inset*2;
        tr::set_color(renderer,light.red,light.green*.55F,light.blue*.18F,1);
        tr::rect(renderer,&target);
        tr::line(renderer,target.x,target.y,target.x+target.w,target.y+target.h);
        tr::line(renderer,target.x+target.w,target.y,target.x,target.y+target.h);
    }
}
