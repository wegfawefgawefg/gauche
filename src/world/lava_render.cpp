#include "lava_render.hpp"

bool draw_lava(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,Cell cell,
                       SDL_FRect rect,const LightingCache& lighting) {
    const auto& tile=game.stage.at_or_border(cell);
    if (tile.kind!=TileKind::Lava) return false;
    const auto tick=tile.surface.still_ticks>0 ? 0 : game.tick;
    SDL_Texture* texture=texture_for(graphics,lava_sprite(tick));
    // One native-resolution pattern spans eight tiles in each direction.
    const SDL_FRect uv{static_cast<float>((cell.x%8+8)%8)/8,
        static_cast<float>((cell.y%8+8)%8)/8,1.0F/8,1.0F/8};
    draw_lit_tile(renderer,texture,rect,cell,lighting,{1,1,1},uv);
    const auto light=lit_sprite_color(lighting,cell);
    const auto fill=[&](int x,int y,int w,int h,SDL_Color color) {
        SDL_SetRenderDrawColorFloat(renderer,light.red*color.r/255.0F,
            light.green*color.g/255.0F,light.blue*color.b/255.0F,1);
        const SDL_FRect part{rect.x+rect.w*static_cast<float>(x)/16,
            rect.y+rect.h*static_cast<float>(y)/16,rect.w*static_cast<float>(w)/16,
            rect.h*static_cast<float>(h)/16};
        SDL_RenderFillRect(renderer,&part);
    };
    // Crust follows the pool boundary, never outlines each molten cell.
    constexpr SDL_Color crust{55,37,30,255},rim{104,49,27,255};
    if (game.stage.at_or_border(cell+Cell{0,-1}).kind!=TileKind::Lava) {
        fill(0,0,16,2,crust);fill(2,2,7,1,rim);
    }
    if (game.stage.at_or_border(cell+Cell{-1,0}).kind!=TileKind::Lava) fill(0,0,2,16,crust);
    if (game.stage.at_or_border(cell+Cell{1,0}).kind!=TileKind::Lava) fill(15,0,1,16,crust);
    if (game.stage.at_or_border(cell+Cell{0,1}).kind!=TileKind::Lava) fill(0,15,16,1,rim);
    // Staggered local domes swell, open and collapse. No gameplay RNG or objects.
    auto seed=static_cast<std::uint64_t>(cell.x*7919+cell.y*104729);
    seed^=seed>>16;seed*=0x45d9f3bU;seed^=seed>>13;
    const int phase=static_cast<int>((tick+seed)%180);
    if (phase>=48) return true;
    const int x=5+static_cast<int>(seed%7),y=5+static_cast<int>((seed/11)%7);
    constexpr SDL_Color hot{234,139,47,255},glow{196,85,25,255};
    if (phase<12) {fill(x,y,2,1,glow);return true;}
    const int radius=phase<24 ? 1 : 2;
    fill(x-radius,y-1,2*radius+1,1,hot);
    fill(x-radius-1,y,1,1,glow);fill(x+radius+1,y,1,1,glow);
    fill(x-radius,y+1,2*radius+1,1,glow);
    fill(x-radius+1,y,2*radius-1,1,phase<32 ? glow : crust);
    if (phase>=36) {
        fill(x-3,y-3,1,1,hot);fill(x+3,y-2,1,1,glow);
    }
    return true;
}
