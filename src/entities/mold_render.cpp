#include "mold_render.hpp"
#include "mold_thief.hpp"

void apply_mold_pose(const Entity& actor,SDL_FRect& rect,double& angle) {
    if (actor.kind!=EntityKind::MoldThief) return;
    if (actor.label_a==MoldGrab) {rect.h*=.8F;rect.y+=rect.h*.2F;}
    if (actor.label_a==MoldSeal) angle+=actor.timer_a/6%2==0 ? -10 : 10;
}
void draw_mold_details(SDL_Renderer* renderer,const GameGraphics& graphics,const Entity& actor,SDL_FRect rect,LightColor light) {
    const bool thief=actor.kind==EntityKind::MoldThief;
    if ((!thief && actor.kind!=EntityKind::CastingMold) || actor.health<=0) return;
    const Item& cargo=actor.ground_item;
    if (cargo.kind!=ItemKind::None && cargo.count>0 && (thief || actor.fixture_open)) {
        SDL_Texture* texture=texture_for(graphics,item_sprite(cargo));
        SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
        SDL_FRect item{rect.x+rect.w*(thief ? .55F : .30F),rect.y+rect.h*.56F,rect.w*.46F,rect.h*.46F};
        SDL_RenderTexture(renderer,texture,nullptr,&item);
        SDL_SetTextureColorModFloat(texture,1,1,1);
    }
    if (!thief || actor.label_a!=MoldSeal) return;
    SDL_FRect bar{rect.x+rect.w*.1F,rect.y-rect.h*.12F,rect.w*.8F,rect.h*.07F};
    SDL_SetRenderDrawColor(renderer,18,20,18,240);SDL_RenderFillRect(renderer,&bar);
    bar.w*=static_cast<float>(180-actor.timer_a)/180;
    SDL_SetRenderDrawColorFloat(renderer,light.red*.95F,light.green*.75F,light.blue*.32F,1);
    SDL_RenderFillRect(renderer,&bar);
}
