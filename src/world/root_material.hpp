#pragma once
#include "../game.hpp"

inline Sprite root_sprite(const Stage& stage,Cell cell) {
    if (stage.at_or_border(cell).kind!=TileKind::Wall) return Sprite::RootCut;
    constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
    int mask=0;
    for (int i=0;i<4;++i) {
        const auto& other=stage.at_or_border(cell+directions[i]);
        if (other.kind==TileKind::Wall && other.material==TileMaterial::Root) mask|=1<<i;
    }
    return static_cast<Sprite>(static_cast<int>(Sprite::Root0)+mask);
}

inline int root_turns(const Stage& stage,Cell cell) {
    if (root_sprite(stage,cell)!=Sprite::Root15) return 0;
    const auto root=[&](Cell c){const auto& t=stage.at_or_border(c);return t.kind==TileKind::Wall && t.material==TileMaterial::Root;};
    const int horizontal=root(cell+Cell{3,0})+root(cell+Cell{-3,0});
    const int vertical=root(cell+Cell{0,3})+root(cell+Cell{0,-3});
    return horizontal>vertical ? 1 : 0;
}
