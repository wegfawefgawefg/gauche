#include "hit_render.hpp"
#include "../particles/system.hpp"
#include "../lighting/render.hpp"
#include <algorithm>
#include <cmath>

SDL_FRect jolted_prop_rect(SDL_FRect rect,Cell cell,const Cosmetics* cosmetics) {
    if (!cosmetics) return rect;
    for (const PropJolt& jolt:cosmetics->prop_jolts) if (jolt.cell==cell) {
        const float age=12-static_cast<float>(jolt.life)+std::clamp(cosmetics->frame_alpha,0.0F,1.0F);
        const float amount=std::cos(age*1.05F)*std::max(0.0F,1-age/12)*.12F;
        rect.x+=static_cast<float>(jolt.direction.x)*rect.w*amount;
        rect.y+=static_cast<float>(jolt.direction.y)*rect.h*amount;
        break;
    }
    return rect;
}

void draw_wood_splits(SDL_Renderer* renderer,const GameGraphics& graphics,
    SDL_FRect rect,Cell cell,int hp,int max_hp,int turns,const LightingCache& lighting) {
    if (hp<=0 || hp>=max_hp) return;
    SDL_Texture* texture=texture_for(graphics,hp*2>max_hp ? Sprite::WoodBruised : Sprite::WoodSplit);
    draw_lit_tile(renderer,texture,rect,cell,lighting,{1,1,1},{0,0,1,1},turns);
}
