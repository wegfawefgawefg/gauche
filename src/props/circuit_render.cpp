#include "circuit_render.hpp"
#include "circuits.hpp"
#include "../surfaces/interaction.hpp"

void draw_wire_connections(tr::Renderer* renderer,const Stage& stage,Cell cell,
                           SDL_FRect rect,LightColor light) {
    // CONNECTIONS: Thin copper arms join real neighboring conductors, including wet banks.
    for (Cell side : {Cell{1,0},{-1,0},{0,1},{0,-1}}) {
        const Tile* tile=stage.at(cell+side);
        if (!tile || prop_blocks(tile->prop) || (!surface_wet(*tile) && !circuit_prop(tile->prop))) continue;
        for (int pass=0;pass<2;++pass) {
            const float width=rect.w*(pass==0 ? 3.0F : 1.0F)/16.0F;
            const float cx=rect.x+rect.w*.5F,cy=rect.y+rect.h*.5F;
            SDL_FRect wire=side.x!=0 ? SDL_FRect{side.x<0 ? rect.x : cx,cy-width*.5F,rect.w*.5F,width} :
                SDL_FRect{cx-width*.5F,side.y<0 ? rect.y : cy,width,rect.h*.5F};
            tr::set_color(renderer,light.red*(pass==0 ? .20F : .70F),
                light.green*(pass==0 ? .13F : .41F),light.blue*(pass==0 ? .09F : .20F),1);
            tr::fill_rect(renderer,&wire);
        }
    }
}
