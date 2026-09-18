#include "light_tower_render.hpp"
#include "light_tower.hpp"
#include <algorithm>
#include <cmath>

void draw_light_tower(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting) {
    const Prop& tower=game.stage.at(cell)->prop;const Cell dir=tower_direction(tower);
    const auto floor=tile_rect(cell,camera,zoom);
    const float progress=tower.growth_ticks ? std::clamp(1-static_cast<float>(tower.growth_ticks)/tower_fall_ticks,0.0F,1.0F) : 0;
    const float theta=.075F+progress*(1.57079633F-.075F);
    float dx=4*floor.w*static_cast<float>(dir.x)*std::sin(theta);
    const float dy=4*floor.w*(static_cast<float>(dir.y)*std::sin(theta)-std::cos(theta));
    if (tower.growth_ticks>tower_fall_ticks) dx+=std::sin(static_cast<float>(game.tick%100)*1.3F)*floor.w*.04F;
    const float width=floor.w*1.5F,height=std::max(floor.w*.2F,std::hypot(dx,dy))*72/64;
    const SDL_FPoint pivot{width*.5F,height*68/72};
    const SDL_FRect body{floor.x+floor.w*.5F-pivot.x,floor.y+floor.h*.6F-pivot.y,width,height};
    const double angle=std::atan2(dx,-dy)*180/3.141592653589793;
    const auto light=light_at_cell(lighting,cell);
    bool obscures=false;
    for (Handle h : controlled_entities(game)) if (const auto* player=get_entity(game,h))
        if (player->health>0 && std::abs(player->cell.x-cell.x)<=1 && player->cell.y<cell.y && player->cell.y>=cell.y-4) obscures=true;
    const bool dark=tower.growth_ticks>0 && (tower.growth_ticks<=tower_fall_ticks || tower.growth_ticks%12<6);
    tr::Texture* texture=texture_for(graphics,tower.hp<50 || tower.growth_ticks ? Sprite::TowerBuckled : Sprite::LightTower);
    tr::texture_color(texture,light.red,light.green,light.blue);tr::texture_alpha_bytes(texture,obscures ? 95 : 255);
    tr::draw_rotated(renderer,texture,nullptr,&body,angle,&pivot,SDL_FLIP_NONE);
    tr::texture_alpha_bytes(texture,255);tr::texture_color(texture,1,1,1);
    // Two lamp faces follow the actual projected head, then go dark at failure.
    const float cx=floor.x+floor.w*.5F+dx,cy=floor.y+floor.h*.6F+dy;
    tr::set_blend(renderer,SDL_BLENDMODE_BLEND);
    if (!dark) {
        tr::Texture* lamps=texture_for(graphics,Sprite::TowerLamps);
        tr::texture_alpha_bytes(lamps,obscures ? 100 : 245);
        tr::draw_rotated(renderer,lamps,nullptr,&body,angle,&pivot,SDL_FLIP_NONE);
        tr::texture_alpha_bytes(lamps,255);
    }
    if (tower.growth_ticks>0 && tower.growth_ticks<=tower_fall_ticks && tower.growth_ticks>tower_fall_ticks-12) {
        tr::set_color_bytes(renderer,249,176,66,220);
        for (int i=0;i<5;++i) {
            const float t=static_cast<float>((game.tick+static_cast<std::uint64_t>(i*3))%12)/12;
            const float x=cx+floor.w*(static_cast<float>(i)-2)*t*.35F,y=cy+floor.h*t*t;
            tr::line(renderer,x,y,x-floor.w*.04F,y-floor.h*.08F);
        }
    }
    tr::set_blend(renderer,SDL_BLENDMODE_NONE);
}
void draw_tower_ground(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting) {
    const Prop& prop=game.stage.at(cell)->prop;const auto floor=tile_rect(cell,camera,zoom);
    if (prop.kind==PropKind::TowerWreck && prop.broken) return;
    const auto light=light_at_cell(lighting,cell);
    tr::Texture* texture=texture_for(graphics,prop.kind==PropKind::TowerWreck ? Sprite::TowerWreck : Sprite::TowerFoot);
    tr::texture_color(texture,light.red,light.green,light.blue);
    tr::draw_rotated(renderer,texture,nullptr,&floor,prop.kind==PropKind::TowerWreck && prop.variant ? 90 : 0,nullptr,SDL_FLIP_NONE);
    tr::texture_color(texture,1,1,1);
    if (prop.kind!=PropKind::LightTower || prop.broken) return;
    const Cell dir=tower_direction(prop);const float x=floor.x+floor.w*.5F,y=floor.y+floor.h*.65F;
    const float tx=x+floor.w*.38F*static_cast<float>(dir.x),ty=y+floor.h*.38F*static_cast<float>(dir.y);
    tr::set_color(renderer,light.red*.85F,light.green*.62F,light.blue*.27F,1);
    tr::line(renderer,x,y,tx,ty);
    for (float sign:{-1.0F,1.0F}) tr::line(renderer,tx,ty,
        tx-floor.w*(static_cast<float>(dir.x)*.15F+static_cast<float>(dir.y)*sign*.14F),
        ty-floor.h*(static_cast<float>(dir.y)*.15F-static_cast<float>(dir.x)*sign*.14F));
    if (!prop.growth_ticks) return;
    tr::set_blend(renderer,SDL_BLENDMODE_BLEND);
    for (int n=1;n<=tower_reach;++n) {
        const Cell target=cell+Cell{dir.x*n,dir.y*n};if (!walkable(game.stage.at_or_border(target).kind)) break;
        SDL_FRect mark=tile_rect(target,camera,zoom);mark.x+=mark.w*.07F;mark.y+=mark.h*.14F;mark.w*=.86F;mark.h*=.72F;
        tr::set_color_bytes(renderer,0,0,0,170);tr::fill_rect(renderer,&mark);
        tr::set_color_bytes(renderer,230,152,62,220);tr::rect(renderer,&mark);
    }
    tr::set_blend(renderer,SDL_BLENDMODE_NONE);
}
