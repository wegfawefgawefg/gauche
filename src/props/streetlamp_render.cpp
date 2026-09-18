#include "streetlamp_render.hpp"
#include "streetlamp.hpp"
#include <algorithm>
#include <cmath>

void draw_streetlamp(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
                     Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting) {
    const Prop& prop=game.stage.at(cell)->prop;
    const SDL_FRect floor=tile_rect(cell,camera,zoom);
    const float pixels=floor.w;
    const float progress=prop.growth_ticks>0 ? std::clamp(1-static_cast<float>(prop.growth_ticks)/pole_fall_ticks,0.0F,1.0F) : 0;
    const float theta=progress*1.57079633F;
    const Cell dir=streetlamp_direction(prop);
    float dx=3*pixels*static_cast<float>(dir.x)*std::sin(theta);
    float dy=3*pixels*(static_cast<float>(dir.y)*std::sin(theta)-std::cos(theta));
    if (prop.growth_ticks>pole_fall_ticks) dx+=std::sin(static_cast<float>(game.tick%100)*1.3F)*pixels*.05F;
    // The bolt plate stays anchored. Foreshorten the length toward the camera,
    // not its width: the lamp head must not disappear when the pole is end-on.
    // Native 16x48 art: narrow pole, foot anchored at pixel (7.5,44).
    const float height=std::max(pixels*.16F,std::hypot(dx,dy))*48/44;
    const float width=pixels;
    const SDL_FPoint pivot{width*7.5F/16,height*44/48};
    SDL_FRect body{floor.x+pixels*.5F-pivot.x,floor.y+pixels*.6F-pivot.y,width,height};
    const double angle=std::atan2(dx,-dy)*180/3.141592653589793;
    tr::Texture* texture=texture_for(graphics,Sprite::StreetLamp);
    const auto light=light_at_cell(lighting,cell);
    tr::texture_color(texture,light.red,light.green,light.blue);
    bool obscures=false;
    for (Handle handle : controlled_entities(game)) if (const Entity* player=get_entity(game,handle))
        if (player->health>0 && std::abs(player->cell.x-cell.x)<=1 && player->cell.y<cell.y && player->cell.y>=cell.y-3) obscures=true;
    tr::texture_alpha_bytes(texture,obscures ? 90 : 255);
    tr::draw_rotated(renderer,texture,nullptr,&body,angle,&pivot,SDL_FLIP_NONE);
    tr::texture_alpha_bytes(texture,255);tr::texture_color(texture,1,1,1);
}
void draw_streetlamp_shadow(tr::Renderer* renderer,const Stage& stage,Cell cell,ViewCamera camera,float zoom) {
    const Prop& prop=stage.at(cell)->prop;
    if (!prop.growth_ticks) return;
    const Cell dir=streetlamp_direction(prop);
    tr::set_blend(renderer,SDL_BLENDMODE_BLEND);
    for (int i=1;i<=pole_reach;++i) {
        const Cell target=cell+Cell{dir.x*i,dir.y*i};
        if (!walkable(stage.at_or_border(target).kind)) break;
        SDL_FRect rect=tile_rect(target,camera,zoom);
        rect.x+=rect.w*.12F;rect.y+=rect.h*.32F;rect.w*=.76F;rect.h*=.36F;
        tr::set_color_bytes(renderer,0,0,0,160);tr::fill_rect(renderer,&rect);
        tr::set_color_bytes(renderer,181,137,71,170);tr::rect(renderer,&rect);
    }
    tr::set_blend(renderer,SDL_BLENDMODE_NONE);
}
void draw_pole_wreck(tr::Renderer* renderer,const Prop& prop,SDL_FRect rect,LightColor light) {
    SDL_FRect bar=rect;
    if (prop.variant==0) {bar.y+=bar.h*.42F;bar.h*=.2F;}
    else {bar.x+=bar.w*.42F;bar.w*=.2F;}
    tr::set_color(renderer,.35F*light.red,.29F*light.green,.25F*light.blue,1);
    tr::fill_rect(renderer,&bar);
    tr::set_color(renderer,.59F*light.red,.51F*light.green,.40F*light.blue,1);
    tr::rect(renderer,&bar);
}
