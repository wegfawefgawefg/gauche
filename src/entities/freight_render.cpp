#include "freight_render.hpp"
#include "rail_shunter.hpp"

void apply_freight_pose(const Entity& actor,SDL_FRect& rect,double& angle) {
    if (actor.kind==EntityKind::RailShunter) {
        if (actor.label_a==ShunterBell) angle+=actor.timer_a/6%2==0 ? -9 : 9;
        if (actor.label_a==ShunterSwing) {rect.y-=rect.h*.12F;rect.h*=1.12F;}
    }
    if (actor.kind==EntityKind::RailCart && actor.label_a==1) rect.y-=rect.h*(actor.timer_b%4<2 ? .025F : 0);
}
void draw_freight_details(SDL_Renderer* renderer,const GameGraphics& graphics,const Entity& actor,SDL_FRect rect,LightColor light) {
    if (actor.kind!=EntityKind::RailCart || actor.health<=0) return;
    if (actor.ground_item.kind!=ItemKind::None && actor.ground_item.count>0) {
        SDL_Texture* texture=texture_for(graphics,item_sprite(actor.ground_item));
        SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
        SDL_FRect cargo{rect.x+rect.w*.28F,rect.y+rect.h*.18F,rect.w*.44F,rect.h*.44F};
        SDL_RenderTexture(renderer,texture,nullptr,&cargo);SDL_SetTextureColorModFloat(texture,1,1,1);
    }
    if (actor.timer_a>0) {
        SDL_FRect brake{rect.x+rect.w*.18F,rect.y+rect.h*.70F,rect.w*.64F,rect.h*.08F};
        SDL_SetRenderDrawColorFloat(renderer,light.red*.92F,light.green*.63F,light.blue*.24F,1);SDL_RenderFillRect(renderer,&brake);
    }
}
