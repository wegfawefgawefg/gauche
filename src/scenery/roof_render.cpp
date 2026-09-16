#include "roof_render.hpp"
#include <algorithm>
#include <array>

bool reveal_roof(const RoofSpan& roof,Cell viewer) {
    // Include both entrance lips and the half-cell roof projection northward.
    // Only this viewport's viewer counts; distant online players cannot reveal it.
    const Cell delta=viewer-roof.start;
    return roof.hp>0 && delta.x>=-1 && delta.y>=-1 &&
        delta.x<=(roof.vertical ? 3 : roof.length) && delta.y<=(roof.vertical ? roof.length : 3);
}

void draw_roofs(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                const Entity* viewer,ViewCamera camera,float zoom,const LightingCache& lighting) {
    constexpr std::array<Sprite,3> bodies{Sprite::RoofLogA,Sprite::RoofIceA,Sprite::RoofGantryA};
    constexpr std::array<Sprite,3> ends{Sprite::RoofLogEndA,Sprite::RoofIceEndA,Sprite::RoofGantryEndA};
    for (const RoofSpan& roof:game.stage.roofs) {
        if (!roof.hp) continue;
        const bool reveal=viewer && reveal_roof(roof,viewer->cell);
        const float condition=static_cast<float>(roof.hp)/static_cast<float>(roof_health(roof.kind));
        for (int along=0;along<roof.length;++along) for (int across=0;across<3;++across) {
            const Cell cell=roof_cell(roof,along,across);
            SDL_FRect rect=tile_rect(cell,camera,zoom);
            rect.y-=rect.h*.5F;
            if (rect.x+rect.w<0 || rect.y+rect.h<0 || rect.x>640 || rect.y>360) continue;
            const bool end=along==0 || along==roof.length-1;
            const auto base=(end ? ends : bodies)[static_cast<std::size_t>(roof.kind)];
            const Sprite sprite=static_cast<Sprite>(static_cast<int>(base)+(roof.vertical ? 2-across : across));
            SDL_Texture* texture=texture_for(graphics,sprite);
            const auto light=light_at_cell(lighting,cell);
            const float charred=roof.kind==RoofKind::Log ? .45F+.55F*condition : 1;
            SDL_SetTextureColorModFloat(texture,light.red*charred,light.green*charred,light.blue*charred);
            SDL_SetTextureAlphaMod(texture,static_cast<Uint8>(reveal ? 42 : roof.kind==RoofKind::IceArch ? 170+70*condition : 245));
            SDL_RenderTextureRotated(renderer,texture,nullptr,&rect,roof.vertical ? 90 : 0,nullptr,
                end && along>0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
            SDL_SetTextureAlphaMod(texture,255);SDL_SetTextureColorModFloat(texture,1,1,1);
        }
    }
}
