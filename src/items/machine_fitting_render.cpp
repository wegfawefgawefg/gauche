#include "machine_fitting_render.hpp"

void draw_machine_fitting(tr::Renderer* renderer,const GameGraphics& graphics,const Entity& machine,SDL_FRect rect,LightColor light) {
    if ((machine.kind!=EntityKind::BoilerTank && machine.kind!=EntityKind::EmergencyPump) ||
        machine.health<=0 || machine.ground_item.kind!=ItemKind::NozzleElbow) return;
    tr::Texture* texture=texture_for(graphics,Sprite::NozzleElbow);
    tr::texture_color(texture,light.red,light.green,light.blue);
    SDL_FRect fitting{rect.x+rect.w*.64F,rect.y+rect.h*.45F,rect.w*.55F,rect.h*.55F};
    tr::draw_rotated(renderer,texture,nullptr,&fitting,0,nullptr,
        machine.ground_item.loaded ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    tr::texture_color(texture,1,1,1);
}
