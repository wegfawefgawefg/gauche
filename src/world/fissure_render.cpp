#include "fissure_render.hpp"
#include "../lighting/render.hpp"
#include <algorithm>

void draw_fissures(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                    ViewCamera camera,float zoom,const LightingCache& lighting,bool plumes) {
    for (const auto& f:game.fissures) for (int n=-1;n<=1;++n) {
        const Cell cell=fissure_cell(f,n);const auto& tile=game.stage.at_or_border(cell);
        if ((tile.kind!=TileKind::Ruin && tile.kind!=TileKind::Empty && tile.kind!=TileKind::Grass) || prop_blocks(tile.prop)) continue;
        SDL_FRect rect=tile_rect(cell,camera,zoom);
        if (rect.x+rect.w<0 || rect.x>640 || rect.y+rect.h<0 || rect.y>420) continue;
        const bool steam=f.kind==FissureKind::Steam,pressure=f.phase==FissurePhase::Pressure;
        const bool release=f.phase==FissurePhase::Release,cooling=f.phase==FissurePhase::Cooling;
        const auto light=light_at_cell(lighting,cell);
        if (!plumes) {
            const auto sprite=cooling ? Sprite::FissureCooled : pressure || release ? Sprite::FissureGlow : Sprite::FissureCrack;
            SDL_Texture* texture=texture_for(graphics,sprite);
            const float glow=release ? .7F : pressure ? .24F+static_cast<float>((game.tick/8)%2)*.3F : 0;
            SDL_SetTextureColorModFloat(texture,std::max(light.red,glow),std::max(light.green,glow*(steam ? .9F : .65F)),std::max(light.blue,glow*(steam ? .8F : .35F)));
            SDL_RenderTextureRotated(renderer,texture,nullptr,&rect,f.axis.y ? 90 : 0,nullptr,SDL_FLIP_NONE);
            SDL_SetTextureColorModFloat(texture,1,1,1);continue;
        }
        if (f.phase==FissurePhase::Idle) continue;
        const bool frame=(game.tick/7+static_cast<std::uint64_t>(cell.x*7+cell.y*11))%2;
        const auto sprite=release && !steam ? (frame ? Sprite::FissureLavaA : Sprite::FissureLavaB) :
            (frame ? Sprite::FissureSteamA : Sprite::FissureSteamB);
        SDL_Texture* texture=texture_for(graphics,sprite);
        const float height=release ? (steam ? 1.8F : 1.3F) : cooling ? .55F : .3F+.35F*(1-static_cast<float>(f.ticks)/fissure_warning_ticks);
        const float width=release ? .9F : .55F;
        rect.x+=rect.w*(1-width)*.5F;rect.y+=rect.h*(.72F-height);rect.w*=width;rect.h*=height;
        SDL_SetTextureColorModFloat(texture,std::max(.2F,light.red),std::max(.2F,light.green),std::max(.2F,light.blue));
        if (release && !steam) SDL_SetTextureColorModFloat(texture,1,1,1);
        SDL_SetTextureAlphaModFloat(texture,release ? .8F : pressure ? .4F : .24F);
        SDL_RenderTexture(renderer,texture,nullptr,&rect);
        SDL_SetTextureColorModFloat(texture,1,1,1);SDL_SetTextureAlphaMod(texture,255);
    }
}
