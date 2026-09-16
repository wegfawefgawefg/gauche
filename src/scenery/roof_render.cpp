#include "roof_render.hpp"
#include <algorithm>
#include <array>

bool reveal_roof(const RoofSpan& roof,Cell viewer) {
    // Only the actual passage and this viewport's viewer reveal the group.
    // Standing beside a support or just outside either entrance does not count.
    const Cell delta=viewer-roof.start;
    const int along=roof.vertical ? delta.y : delta.x;
    const int across=roof.vertical ? delta.x : delta.y;
    return roof.hp>0 && along>=0 && along<roof.length && across==1;
}

std::optional<Sprite> log_far_support(const Stage& stage,Cell cell) {
    // The ground supports share the dome silhouette, otherwise square timber
    // corners poke through its transparent pixels (especially when revealed).
    for (const RoofSpan& roof:stage.roofs) {
        if (!roof.hp || roof.kind!=RoofKind::Log || !roof.vertical || cell.y!=roof.start.y) continue;
        const int across=cell.x-roof.start.x;
        if (across==0 || across==2) return static_cast<Sprite>(static_cast<int>(Sprite::RoofLogFarA)+across);
    }
    return {};
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
            // Timber caps and their support walls share the same tile anchors.
            if (roof.kind!=RoofKind::Log) rect.y-=rect.h*.5F;
            if (rect.x+rect.w<0 || rect.y+rect.h<0 || rect.x>640 || rect.y>360) continue;
            const bool end=along==0 || along==roof.length-1;
            const bool upright_cap=roof.kind==RoofKind::Log && roof.vertical && end;
            const auto base=upright_cap ? (along==0 ? Sprite::RoofLogFarA : Sprite::RoofLogNearA) :
                (end ? ends : bodies)[static_cast<std::size_t>(roof.kind)];
            const Sprite sprite=static_cast<Sprite>(static_cast<int>(base)+(roof.vertical && !upright_cap ? 2-across : across));
            SDL_Texture* texture=texture_for(graphics,sprite);
            const auto light=light_at_cell(lighting,cell);
            const float charred=roof.kind==RoofKind::Log ? .45F+.55F*condition : 1;
            SDL_SetTextureColorModFloat(texture,light.red*charred,light.green*charred,light.blue*charred);
            SDL_SetTextureAlphaMod(texture,static_cast<Uint8>(reveal ? 42 : roof.kind==RoofKind::IceArch ? 170+70*condition : 245));
            SDL_RenderTextureRotated(renderer,texture,nullptr,&rect,roof.vertical && !upright_cap ? 90 : 0,nullptr,
                end && along>0 && !upright_cap ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
            SDL_SetTextureAlphaMod(texture,255);SDL_SetTextureColorModFloat(texture,1,1,1);
        }
    }
}
