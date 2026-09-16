#include "roof_render.hpp"
#include "hollow_tree_render.hpp"
#include "ice_arch_render.hpp"
#include "../world/terrain_material.hpp"
#include <algorithm>
#include <array>

bool reveal_roof(const RoofSpan& roof,Cell viewer) {
    // Giant crowns extend above/beyond their rooms; visibility follows their
    // screen projection. Narrow passages still reveal only actual occupants.
    if (roof.kind==RoofKind::HollowTree) return roof.hp>0 && hollow_tree_opacity(roof,viewer)<1;
    const Cell delta=viewer-roof.start;
    const int along=roof.vertical ? delta.y : delta.x;
    const int across=roof.vertical ? delta.x : delta.y;
    return roof.hp>0 && along>=0 && along<roof.length && across==1 &&
        (roof.kind!=RoofKind::IceArch || (along>0 && along<roof.length-1));
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

void draw_roof_row(SDL_Renderer* renderer,const GameGraphics& graphics,const Stage& stage,const RoofSpan& roof,int row,
                   const Entity* viewer,ViewCamera camera,float zoom,const LightingCache& lighting) {
    constexpr std::array<Sprite,4> bodies{Sprite::RoofLogA,Sprite::RoofFrozenLogA,Sprite::RoofGantryA,Sprite::RoofContainerA};
    constexpr std::array<Sprite,4> ends{Sprite::RoofLogEndA,Sprite::RoofFrozenLogEndA,Sprite::RoofGantryEndA,Sprite::RoofContainerEndA};
    if (!roof.hp || row<0 || row>=roof_rows(roof)) return;
    if (roof.kind==RoofKind::HollowTree) {draw_hollow_tree_row(renderer,graphics,stage,roof,row,viewer,camera,zoom,lighting);return;}
    if (roof.kind==RoofKind::IceArch) {draw_ice_arch_row(renderer,graphics,stage,roof,row,viewer,camera,zoom,lighting);return;}
    const bool reveal=viewer && reveal_roof(roof,viewer->cell);
    const float condition=static_cast<float>(roof.hp)/static_cast<float>(roof_health(roof.kind));
    for (int column=0;column<(roof.vertical ? 3 : roof.length);++column) {
        const int along=roof.vertical ? row : column;
        const int across=roof.vertical ? column : row;
        const Cell cell=roof_cell(roof,along,across);
        SDL_FRect rect=tile_rect(cell,camera,zoom);
        // Timber caps and their support walls share the same tile anchors.
        if (roof.kind!=RoofKind::Log && roof.kind!=RoofKind::Container) rect.y-=rect.h*.5F;
        if (rect.x+rect.w<0 || rect.y+rect.h<0 || rect.x>640 || rect.y>360) continue;
        const bool end=along==0 || along==roof.length-1;
        const bool upright_cap=(roof.kind==RoofKind::Log || roof.kind==RoofKind::Container) && roof.vertical && end;
        const auto cap=roof.kind==RoofKind::Container ?
            (along==0 ? Sprite::RoofContainerFarA : Sprite::RoofContainerNearA) :
            (along==0 ? Sprite::RoofLogFarA : Sprite::RoofLogNearA);
        const auto base=upright_cap ? cap :
            (end ? ends : bodies)[static_cast<std::size_t>(roof.kind)];
        const Sprite sprite=static_cast<Sprite>(static_cast<int>(base)+(roof.vertical && !upright_cap ? 2-across : across));
        SDL_Texture* texture=texture_for(graphics,sprite);
        const float charred=roof.kind==RoofKind::Log ? .45F+.55F*condition : 1;
        const float opacity=reveal ? 42.0F/255 : roof.kind==RoofKind::FrozenLog ? (170+70*condition)/255 : 1;
        // A roof is one surface: shared corner lighting avoids cell-sized
        // brightness blocks. Only the actual occupant changes its opacity.
        draw_lit_tile(renderer,texture,rect,cell,lighting,{charred,charred,charred},{0,0,1,1},
            roof.vertical && !upright_cap ? 1 : 0,opacity,end && along>0 && !upright_cap);
    }
}

void draw_roofs(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                const Entity* viewer,ViewCamera camera,float zoom,const LightingCache& lighting) {
    for (const auto& roof:game.stage.roofs)
        for (int row=0;row<roof_rows(roof);++row)
            draw_roof_row(renderer,graphics,game.stage,roof,row,viewer,camera,zoom,lighting);
}

std::optional<RoofGround> roof_ground(const Stage& stage,Cell cell) {
    // Ground stays after the roof breaks. Draw with terrain, beneath surfaces.
    if (!walkable(stage.at_or_border(cell).kind)) return {};
    for (const auto& roof:stage.roofs) {
        if (roof.kind!=RoofKind::Log && roof.kind!=RoofKind::Container) continue;
        const Cell delta=cell-roof.start;
        const int along=roof.vertical ? delta.y : delta.x;
        const int across=roof.vertical ? delta.x : delta.y;
        if (along>=0 && along<roof.length && across==1)
            return RoofGround{roof.kind==RoofKind::Log ? Sprite::LogFloor : Sprite::ContainerFloor,roof.vertical ? 1 : 0};
    }
    return {};
}

int log_support_turns(const Stage& stage,Cell cell) {
    if (!wooden_terrain(stage.at_or_border(cell))) return 0;
    for (const auto& roof:stage.roofs) {
        if (roof.kind!=RoofKind::Log || roof.vertical) continue;
        const Cell delta=cell-roof.start;
        if (delta.x>=0 && delta.x<roof.length && (delta.y==0 || delta.y==2)) return 1;
    }
    return 0;
}
