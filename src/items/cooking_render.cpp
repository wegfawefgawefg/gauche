#include "cooking_render.hpp"
#include "cooking.hpp"
#include "../ui/text.hpp"

void draw_cooking(SDL_Renderer* renderer,const GameGraphics& graphics,
                  const Entity& player,SDL_FRect rect,LightColor light) {
    if (!cooking_action(player) || player.health<=0 || player.counter_b==2) return;
    SDL_FRect food{rect.x+rect.w*(static_cast<float>(player.point_b.x-player.cell.x)+.22F),
        rect.y+rect.h*(static_cast<float>(player.point_b.y-player.cell.y)-.05F),rect.w*.56F,rect.h*.56F};
    SDL_Texture* texture=texture_for(graphics,item_sprite(player.ground_item));
    SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
    SDL_RenderTextureRotated(renderer,texture,nullptr,&food,player.counter_a/6%2==0 ? -12 : 12,nullptr,SDL_FLIP_NONE);
    SDL_SetTextureColorModFloat(texture,1,1,1);
    const bool full=player.counter_a>=cooking_ticks;
    SDL_FRect bar{food.x,food.y+food.h+1,food.w,1};
    SDL_SetRenderDrawColorFloat(renderer,light.red*.22F,light.green*.18F,light.blue*.1F,1);
    SDL_RenderFillRect(renderer,&bar);
    bar.w*=static_cast<float>(player.counter_a)/cooking_ticks;
    SDL_SetRenderDrawColorFloat(renderer,light.red,light.green*(full ? .25F : .7F),light.blue*.22F,1);
    SDL_RenderFillRect(renderer,&bar);
    if (full) small_ui_text(renderer,food.x-8,food.y-7,"NO ROOM",220,126,85);
}
