#include "rail_render.hpp"
#include "rail_points.hpp"

bool draw_freight_track(SDL_Renderer* renderer,const GameGraphics& graphics,const Stage& stage,
                        Cell cell,SDL_FRect rect,const LightingCache& lighting) {
    const Tile& tile=stage.at_or_border(cell);
    if (tile.kind!=TileKind::Rail || tile.max_hp==0) return false;
    draw_lit_tile(renderer,texture_for(graphics,Sprite::IndustrialFloorA),rect,cell,lighting,{1,1,1});
    const LightColor light=light_at_cell(lighting,cell);
    const auto box=[&](float x,float y,float w,float h,LightColor color) {
        SDL_SetRenderDrawColorFloat(renderer,color.red*light.red,color.green*light.green,color.blue*light.blue,1);
        SDL_FRect piece{rect.x+x*rect.w/16,rect.y+y*rect.h/16,w*rect.w/16,h*rect.h/16};
        SDL_RenderFillRect(renderer,&piece);
    };
    // Half-segments meet at the centre; missing neighbours leave real gaps.
    // Direction derives only from track topology, never camera or cart facing.
    for (Cell dir:rail_directions) {
        if (stage.at_or_border(cell+dir).kind!=TileKind::Rail) continue;
        if (dir.x!=0) {
            const float start=dir.x>0 ? 8.0F : 0.0F;
            for (float x=start+1;x<start+8;x+=4) box(x,3,2,10,{.24F,.19F,.13F});
            box(start,5,8,1,{.47F,.49F,.48F});box(start,10,8,1,{.35F,.38F,.38F});
        } else {
            const float start=dir.y>0 ? 8.0F : 0.0F;
            for (float y=start+1;y<start+8;y+=4) box(3,y,10,2,{.24F,.19F,.13F});
            box(5,start,1,8,{.47F,.49F,.48F});box(10,start,1,8,{.35F,.38F,.38F});
        }
    }
    return true;
}
void draw_rail_points(SDL_Renderer* renderer,const GameGraphics& graphics,const Prop& prop,
                      SDL_FRect rect,LightColor light) {
    auto* texture=texture_for(graphics,Sprite::RailPoints);
    SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
    const SDL_FRect lever{rect.x+rect.w*.2F,rect.y+rect.h*.2F,rect.w*.6F,rect.h*.6F};
    SDL_RenderTextureRotated(renderer,texture,nullptr,&lever,static_cast<double>(prop.variant&3U)*90,nullptr,SDL_FLIP_NONE);
    SDL_SetTextureColorModFloat(texture,1,1,1);
    const Cell dir=rail_directions[prop.variant&3U];
    const float cx=rect.x+rect.w*.5F,cy=rect.y+rect.h*.5F;
    const float tx=cx+static_cast<float>(dir.x)*rect.w*.45F,ty=cy+static_cast<float>(dir.y)*rect.h*.45F;
    SDL_SetRenderDrawColorFloat(renderer,.8F*light.red,.59F*light.green,.24F*light.blue,1);
    for (int sign:{-1,1})
        SDL_RenderLine(renderer,tx,ty,tx-static_cast<float>(dir.x+sign*dir.y)*rect.w*.14F,
                       ty-static_cast<float>(dir.y-sign*dir.x)*rect.h*.14F);
}
