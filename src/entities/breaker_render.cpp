#include "breaker_render.hpp"
#include "strikebreaker.hpp"
#include <cmath>

void draw_breaker_shield(SDL_Renderer* renderer,const GameGraphics& graphics,
    const Entity& guard,SDL_FRect rect,LightColor light) {
    if (guard.health<=0 || guard.counter_c<=0) return;
    const bool raised=breaker_blocks(guard,guard.cell+guard.facing);
    const float size=raised ? .64F : .48F;
    const float forward=guard.label_a==BreakerPush ? .42F : raised ? .32F : .16F;
    SDL_FRect shield{rect.x+rect.w*(.5F-size*.5F+static_cast<float>(guard.facing.x)*forward),
        rect.y+rect.h*(.55F-size*.5F+static_cast<float>(guard.facing.y)*forward+(raised ? 0 : .17F)),
        rect.w*size,rect.h*size};
    auto* texture=texture_for(graphics,Sprite::BreakerShield);
    SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
    const double angle=std::atan2(static_cast<double>(guard.facing.y),static_cast<double>(guard.facing.x))*180/3.141592653589793;
    SDL_RenderTextureRotated(renderer,texture,nullptr,&shield,angle,nullptr,SDL_FLIP_NONE);
    SDL_SetTextureColorModFloat(texture,1,1,1);
}
