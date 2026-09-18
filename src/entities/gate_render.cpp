#include "gate_render.hpp"

void draw_gate(tr::Renderer* renderer,const GameGraphics& graphics,const Entity& gate,
    ViewCamera camera,float zoom,const LightingCache& lighting) {
    const bool warning=gate.label_b==1 && gate.label_a==0 && gate.timer_a<=30;
    const Sprite sprite=gate.fixture_open ? Sprite::SluiceOpen : gate.sprite;
    tr::Texture* texture=texture_for(graphics,sprite);
    SDL_FRect rect=tile_rect(gate.cell,camera,zoom);
    const LightColor light=lit_sprite_color(lighting,gate.cell);
    tr::texture_color(texture,light.red,light.green,light.blue);
    tr::draw_texture(renderer,texture,nullptr,&rect);
    tr::texture_color(texture,1,1,1);
    // SIGNAL: A small amber blink precedes a timed closure, without a debug overlay.
    const bool amber=warning && (gate.timer_a/5)%2==0;
    const SDL_FRect lamp{rect.x+rect.w*.78F,rect.y+rect.h*.12F,rect.w*.1F,rect.h*.1F};
    tr::set_color(renderer,
        light.red*(amber ? .92F : gate.fixture_open ? .37F : .71F),
        light.green*(amber ? .70F : gate.fixture_open ? .61F : .24F),
        light.blue*(amber ? .22F : .25F),1);
    tr::fill_rect(renderer,&lamp);
}
