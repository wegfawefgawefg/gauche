#include "hollow_tree_render.hpp"
#include "hollow_tree.hpp"
#include "../world/terrain_material.hpp"
#include <algorithm>
#include <cmath>

float hollow_tree_opacity(const RoofSpan& roof,Cell viewer) {
    if (!roof.hp) return 0;
    const float rx=static_cast<float>(roof.length)*.5F,ry=static_cast<float>(roof.width)*.5F;
    const float cx=static_cast<float>(roof.start.x)+rx-.5F,cy=static_cast<float>(roof.start.y)+ry-.5F;
    // The crown is drawn height cells north of its ground footprint. Include
    // that entire projection and the trunk between them, not only the interior.
    const float y=static_cast<float>(viewer.y);
    const float nearest_y=std::clamp(y,cy-static_cast<float>(roof.height),cy);
    const float dx=(static_cast<float>(viewer.x)-cx)/rx,dy=(y-nearest_y)/ry;
    const float outside=(std::sqrt(dx*dx+dy*dy)-1)*std::min(rx,ry);
    // Keep a one-cell margin for the actor's sprite and the immediate approach;
    // restore the crown over three more cells, instead of concealing one step out.
    const float t=std::clamp((outside-1)/3,0.0F,1.0F);
    return .14F+.86F*t*t*(3-2*t);
}

namespace {
void crown(tr::Renderer* renderer,tr::Texture* texture,SDL_FRect rect,Cell cell,SDL_FRect uv,
    const LightingCache& lighting,float alpha,float condition) {
    // The upper foliage receives a little skylight even above an unlit trunk.
    const auto tint=[&](Cell corner) {
        const auto light=light_at_corner(lighting,corner);
        const float charred=.35F+.65F*condition;
        return SDL_FColor{std::max(.16F,light.red)*charred,std::max(.20F,light.green)*charred,
            std::max(.12F,light.blue)*charred,alpha};
    };
    const SDL_Vertex vertices[]{
        {{rect.x,rect.y},tint(cell),{uv.x,uv.y}},
        {{rect.x+rect.w,rect.y},tint(cell+Cell{1,0}),{uv.x+uv.w,uv.y}},
        {{rect.x+rect.w,rect.y+rect.h},tint(cell+Cell{1,1}),{uv.x+uv.w,uv.y+uv.h}},
        {{rect.x,rect.y+rect.h},tint(cell+Cell{0,1}),{uv.x,uv.y+uv.h}}};
    constexpr int indices[]{0,1,2,0,2,3};
    tr::geometry(renderer,texture,vertices,4,indices,6);
}
}
void draw_hollow_tree_row(tr::Renderer* renderer,const GameGraphics& graphics,const Stage& stage,
    const RoofSpan& roof,int row,const Entity* viewer,ViewCamera camera,float zoom,const LightingCache& lighting) {
    const float opacity=viewer ? hollow_tree_opacity(roof,viewer->cell) : 1;
    const float condition=static_cast<float>(roof.hp)/static_cast<float>(roof_health(roof.kind));
    auto* leaves=texture_for(graphics,Sprite::GiantTreeCanopy);
    for (int x=0;x<roof.length;++x) {
        const Cell c=roof.start+Cell{x,row};
        SDL_FRect ground=tile_rect(c,camera,zoom),top=ground;top.y-=ground.h*roof.height;
        if (top.x+top.w<0 || top.x>640 || ground.y+ground.h<0 || top.y>360) continue;
        const auto& tile=stage.at_or_border(c);
        // South-facing standing bark reaches from the canopy edge to its base.
        // Cut entrances remove their own face; intact ground walls remain solid.
        const bool edge=tree_ellipse(roof,c) && !tree_ellipse(roof,c+Cell{0,1});
        if (edge && tile.kind==TileKind::Wall && wooden_terrain(tile)) {
            SDL_FRect face=top;face.h=ground.h*(roof.height+1);
            draw_lit_tile(renderer,texture_for(graphics,Sprite::GiantTreeBark),face,c,lighting,
                {condition,condition,condition},{0,0,1,1},0,opacity);
        }
        const SDL_FRect uv{static_cast<float>(x)/roof.length,static_cast<float>(row)/roof.width,
            1.0F/roof.length,1.0F/roof.width};
        crown(renderer,leaves,top,c,uv,lighting,opacity,condition);
    }
}
