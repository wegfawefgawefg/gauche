#include "ice_material_render.hpp"
#include "water.hpp"

void draw_ice_material_details(SDL_Renderer* renderer,const GameGraphics& graphics,
    const Stage& stage,Cell cell,SDL_FRect rect,const LightingCache& lighting) {
    const Tile& tile=stage.at_or_border(cell);
    const LightColor light=lit_sprite_color(lighting,cell);
    const float pixel=rect.w/16;
    // A sparse two-pixel lip follows actual material boundaries. Never paint
    // tile-center highlights: they made the old lighting look like a checkerboard.
    for (Cell side:{Cell{1,0},{-1,0},{0,1},{0,-1}}) {
        const Tile& other=stage.at_or_border(cell+side);
        const bool snow=tile.kind==TileKind::Snow && other.kind!=TileKind::Snow && other.kind!=TileKind::Wall;
        const bool shore=(shallow_water(tile.kind) || tile.kind==TileKind::Water) &&
            (other.kind==TileKind::Snow || other.kind==TileKind::Ice);
        const bool seam=tile.kind==TileKind::Wall && other.kind==TileKind::Wall && tile.material!=other.material;
        if (!snow && !shore && !seam) continue;
        SDL_SetRenderDrawColorFloat(renderer,light.red*.32F,light.green*.43F,light.blue*.48F,1);
        for (int i=0;i<16;++i) {
            const int inset=(i+cell.x+cell.y)%5==0 ? 2 : 1;
            SDL_FRect mark=side.x!=0 ? SDL_FRect{rect.x+static_cast<float>(side.x>0 ? 16-inset : 0)*pixel,rect.y+static_cast<float>(i)*pixel,pixel, pixel} :
                SDL_FRect{rect.x+static_cast<float>(i)*pixel,rect.y+static_cast<float>(side.y>0 ? 16-inset : 0)*pixel,pixel,pixel};
            SDL_RenderFillRect(renderer,&mark);
        }
    }
    if (tile.kind!=TileKind::Wall || tile.contents==ItemKind::None) return;
    SDL_Texture* item=texture_for(graphics,item_sprite(tile.contents));
    const SDL_FRect inside{rect.x+rect.w*.22F,rect.y+rect.h*.2F,rect.w*.56F,rect.h*.56F};
    SDL_SetTextureColorModFloat(item,light.red*.65F,light.green*.83F,light.blue*.9F);
    SDL_SetTextureAlphaModFloat(item,.75F);
    SDL_RenderTexture(renderer,item,nullptr,&inside);
    SDL_SetTextureAlphaModFloat(item,1);SDL_SetTextureColorModFloat(item,1,1,1);
    // Frost cuts across the object silhouette so it reads as enclosed, not a
    // pickup lying on an ordinary floor. Actual digging/melting releases it.
    SDL_SetRenderDrawColorFloat(renderer,light.red*.38F,light.green*.56F,light.blue*.62F,1);
    SDL_RenderLine(renderer,rect.x+pixel*4,rect.y+pixel*11,rect.x+pixel*11,rect.y+pixel*4);
    SDL_RenderLine(renderer,rect.x+pixel*8,rect.y+pixel*10,rect.x+pixel*12,rect.y+pixel*6);
}
