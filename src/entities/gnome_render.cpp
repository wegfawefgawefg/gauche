#include "gnome_render.hpp"
void draw_tall_mushroom(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting,const Entity* viewer) {
    const auto& prop=game.stage.at(cell)->prop;const auto ground=tile_rect(cell,camera,zoom);
    const float height=prop.variant>=3 ? 2.5F : 1.7F,width=prop.variant>=3 ? 1.7F : 1.3F;
    SDL_FRect rect{ground.x+ground.w*(1-width)*.5F,ground.y+ground.h*(1-height),ground.w*width,ground.h*height};
    auto* texture=texture_for(graphics,static_cast<Sprite>(static_cast<int>(Sprite::TallMushroom)+prop.variant%3));
    const auto light=light_at_cell(lighting,cell);tr::texture_color(texture,light.red,light.green,light.blue);
    const bool behind=viewer && viewer->cell.x==cell.x && viewer->cell.y<cell.y && viewer->cell.y>=cell.y-(prop.variant>=3 ? 2 : 1);
    tr::texture_alpha_bytes(texture,behind ? 130 : 255);tr::draw_texture(renderer,texture,nullptr,&rect);
    tr::texture_alpha_bytes(texture,255);tr::texture_color(texture,1,1,1);
}
