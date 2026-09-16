#include "chasm_render.hpp"
#include "../items/folded_bridge.hpp"

namespace {
bool void_tile(const Tile& tile) {
    return tile.kind==TileKind::Chasm || (tile.kind==TileKind::Bridge && bridge_underlay(tile.prop)==TileKind::Chasm);
}
}

bool draw_chasm(SDL_Renderer* renderer,const Game& game,Cell cell,SDL_FRect rect,const LightingCache& lighting) {
    if (!void_tile(game.stage.at_or_border(cell))) return false;
    const LightColor light=lit_sprite_color(lighting,cell);
    const bool ice=ice_floor(game.run.floor);
    const auto fill=[&](float x,float y,float w,float h,float value) {
        SDL_SetRenderDrawColorFloat(renderer,light.red*value*(ice ? .75F : 1),
            light.green*value*(ice ? .9F : .92F),light.blue*value,1);
        const SDL_FRect part{rect.x+rect.w*x,rect.y+rect.h*y,rect.w*w,rect.h*h};
        SDL_RenderFillRect(renderer,&part);
    };
    draw_flat_tile(renderer,nullptr,rect,{light.red*.012F*(ice ? .75F : 1),
        light.green*.012F*(ice ? .9F : .92F),light.blue*.012F,1});
    const Tile& north=game.stage.at_or_border(cell+Cell{0,-1});
    if (!void_tile(north)) {
        // A lit lip above a tapering dark face reads as missing floor, not wall.
        fill(0,0,1,1.0F/16,.44F);
        for (int row=1;row<=6;++row) fill(0,static_cast<float>(row)/16,1,1.0F/16,.24F*(1-static_cast<float>(row)/7));
        const float cut=static_cast<float>((cell.x*7+cell.y*3)%12+2)/16;
        fill(cut,1.0F/16,1.0F/16,4.0F/16,.022F);
    }
    if (!void_tile(game.stage.at_or_border(cell+Cell{-1,0}))) fill(0,0,1.0F/16,1,.19F);
    if (!void_tile(game.stage.at_or_border(cell+Cell{1,0}))) fill(15.0F/16,0,1.0F/16,1,.12F);
    if (!void_tile(game.stage.at_or_border(cell+Cell{0,1}))) fill(0,15.0F/16,1,1.0F/16,.18F);
    return true;
}
