#include "ice_pillar_render.hpp"
#include "ice_pillar.hpp"
#include <algorithm>
#include <cmath>

void draw_ice_pillar(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                     Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting) {
    const Prop& prop=game.stage.at(cell)->prop;
    const SDL_FRect floor=tile_rect(cell,camera,zoom);
    const float scale=static_cast<float>(pillar_mass(prop))/pillar_melt_steps;
    const float progress=prop.growth_ticks ? std::clamp(1-static_cast<float>(prop.growth_ticks)/pillar_fall_ticks,0.0F,1.0F) : 0;
    const float theta=progress*1.57079633F;
    const Cell dir=pillar_direction(prop);
    const float length=static_cast<float>(pillar_height(prop))*floor.w*scale;
    float dx=length*static_cast<float>(dir.x)*std::sin(theta);
    const float dy=length*(static_cast<float>(dir.y)*std::sin(theta)-std::cos(theta));
    if (prop.growth_ticks>pillar_fall_ticks) dx+=std::sin(static_cast<float>(game.tick%100)*1.7F)*floor.w*.06F;
    const float width=floor.w*(.5F+.5F*scale),height=std::max(floor.w*.13F,std::hypot(dx,dy))*48/44;
    const SDL_FPoint pivot{width*.5F,height*44/48};
    SDL_FRect body{floor.x+floor.w*.5F-pivot.x,floor.y+floor.h*.6F-pivot.y,width,height};
    const bool cracked=prop.hp<prop_max_health(prop)/2 || prop.growth_ticks;
    const auto family=prop.variant>>6;
    const Sprite sprite=family==2 ? (cracked ? Sprite::IcePillarForkedCracked : Sprite::IcePillarForked) :
        family==3 ? (cracked ? Sprite::IcePillarBroadCracked : Sprite::IcePillarBroad) :
        cracked ? Sprite::IcePillarCracked : Sprite::IcePillar;
    SDL_Texture* texture=texture_for(graphics,sprite);
    const auto light=light_at_cell(lighting,cell);
    SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
    bool obscures=false;
    for (Handle h:game.players) if (const Entity* player=get_entity(game,h))
        if (player->health>0 && std::abs(player->cell.x-cell.x)<=1 && player->cell.y<cell.y && player->cell.y>=cell.y-pillar_height(prop)) obscures=true;
    SDL_SetTextureAlphaMod(texture,obscures ? 105 : 235);
    SDL_RenderTextureRotated(renderer,texture,nullptr,&body,std::atan2(dx,-dy)*180/3.141592653589793,&pivot,SDL_FLIP_NONE);
    SDL_SetTextureAlphaMod(texture,255);SDL_SetTextureColorModFloat(texture,1,1,1);
}
void draw_pillar_shadow(SDL_Renderer* renderer,const Stage& stage,Cell cell,ViewCamera camera,float zoom) {
    const Prop& prop=stage.at(cell)->prop;
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_FRect base=tile_rect(cell,camera,zoom);base.x+=base.w*.1F;base.y+=base.h*.48F;base.w*=.8F;base.h*=.22F;
    SDL_SetRenderDrawColor(renderer,0,0,0,150);SDL_RenderFillRect(renderer,&base);
    if (prop.growth_ticks) {
        const Cell dir=pillar_direction(prop);
        for (int i=1;i<=pillar_reach(prop);++i) {
            const Cell target=cell+Cell{dir.x*i,dir.y*i};
            if (!walkable(stage.at_or_border(target).kind)) break;
            SDL_FRect rect=tile_rect(target,camera,zoom);rect.x+=rect.w*.12F;rect.y+=rect.h*.22F;rect.w*=.76F;rect.h*=.56F;
            SDL_SetRenderDrawColor(renderer,0,0,0,170);SDL_RenderFillRect(renderer,&rect);
            SDL_SetRenderDrawColor(renderer,166,204,217,200);SDL_RenderRect(renderer,&rect);
        }
    }
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
}
