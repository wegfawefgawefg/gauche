#include "conveyor_render.hpp"
#include "conveyor.hpp"
void draw_conveyor(SDL_Renderer* renderer,const GameGraphics& graphics,const Prop& prop,
                   SDL_FRect rect,LightColor light,std::uint64_t tick) {
    const bool stopped=prop.growth_ticks>0 || (prop.variant&belt_manual)!=0;
    const Sprite sprite=prop.growth_ticks>0 ? Sprite::ConveyorBrake : stopped ? Sprite::ConveyorOff :
        (tick/10)%2 ? Sprite::ConveyorMoving : Sprite::Conveyor;
    auto* texture=texture_for(graphics,sprite);
    SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
    SDL_RenderTextureRotated(renderer,texture,nullptr,&rect,static_cast<double>(prop.variant&3U)*90,nullptr,SDL_FLIP_NONE);
    SDL_SetTextureColorModFloat(texture,1,1,1);
}
