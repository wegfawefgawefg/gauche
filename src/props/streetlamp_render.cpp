#include "streetlamp_render.hpp"
#include "streetlamp.hpp"
#include <algorithm>
#include <cmath>

void draw_streetlamp(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
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
    SDL_Texture* texture=texture_for(graphics,Sprite::StreetLamp);
    const auto light=light_at_cell(lighting,cell);
    SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
    bool obscures=false;
    for (Handle handle : controlled_entities(game)) if (const Entity* player=get_entity(game,handle))
        if (player->health>0 && std::abs(player->cell.x-cell.x)<=1 && player->cell.y<cell.y && player->cell.y>=cell.y-3) obscures=true;
    SDL_SetTextureAlphaMod(texture,obscures ? 90 : 255);
    SDL_RenderTextureRotated(renderer,texture,nullptr,&body,angle,&pivot,SDL_FLIP_NONE);
    SDL_SetTextureAlphaMod(texture,255);SDL_SetTextureColorModFloat(texture,1,1,1);
}
void draw_streetlamp_shadow(SDL_Renderer* renderer,const Stage& stage,Cell cell,ViewCamera camera,float zoom) {
    const Prop& prop=stage.at(cell)->prop;
    if (!prop.growth_ticks) return;
    const Cell dir=streetlamp_direction(prop);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    for (int i=1;i<=pole_reach;++i) {
        const Cell target=cell+Cell{dir.x*i,dir.y*i};
        if (!walkable(stage.at_or_border(target).kind)) break;
        SDL_FRect rect=tile_rect(target,camera,zoom);
        rect.x+=rect.w*.12F;rect.y+=rect.h*.32F;rect.w*=.76F;rect.h*=.36F;
        SDL_SetRenderDrawColor(renderer,0,0,0,160);SDL_RenderFillRect(renderer,&rect);
        SDL_SetRenderDrawColor(renderer,181,137,71,170);SDL_RenderRect(renderer,&rect);
    }
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
}
void draw_pole_wreck(SDL_Renderer* renderer,const Prop& prop,SDL_FRect rect,LightColor light) {
    SDL_FRect bar=rect;
    if (prop.variant==0) {bar.y+=bar.h*.42F;bar.h*=.2F;}
    else {bar.x+=bar.w*.42F;bar.w*=.2F;}
    SDL_SetRenderDrawColorFloat(renderer,.35F*light.red,.29F*light.green,.25F*light.blue,1);
    SDL_RenderFillRect(renderer,&bar);
    SDL_SetRenderDrawColorFloat(renderer,.59F*light.red,.51F*light.green,.40F*light.blue,1);
    SDL_RenderRect(renderer,&bar);
}
