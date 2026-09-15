#include "machine_fitting_render.hpp"

void draw_machine_fitting(SDL_Renderer* renderer,const GameGraphics& graphics,const Entity& machine,SDL_FRect rect,LightColor light) {
    if ((machine.kind!=EntityKind::BoilerTank && machine.kind!=EntityKind::EmergencyPump) ||
        machine.health<=0 || machine.ground_item.kind!=ItemKind::NozzleElbow) return;
    SDL_Texture* texture=texture_for(graphics,Sprite::NozzleElbow);
    SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
    SDL_FRect fitting{rect.x+rect.w*.64F,rect.y+rect.h*.45F,rect.w*.55F,rect.h*.55F};
    SDL_RenderTextureRotated(renderer,texture,nullptr,&fitting,0,nullptr,
        machine.ground_item.loaded ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    SDL_SetTextureColorModFloat(texture,1,1,1);
}
