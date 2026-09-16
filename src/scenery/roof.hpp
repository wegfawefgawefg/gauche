#pragma once
#include "../game.hpp"

constexpr int max_roof_spans=16;
inline int roof_health(RoofKind kind) {return kind==RoofKind::Log ? 40 : kind==RoofKind::IceArch ? 24 : 80;}
inline Cell roof_cell(const RoofSpan& roof,int along,int across) {
    return roof.start+(roof.vertical ? Cell{across,along} : Cell{along,across});
}
inline bool roof_covers(const RoofSpan& roof,Cell cell) {
    const Cell delta=cell-roof.start;
    const int along=roof.vertical ? delta.y : delta.x,across=roof.vertical ? delta.x : delta.y;
    return roof.hp>0 && along>=0 && along<roof.length && across>=0 && across<3;
}
inline bool valid_roof(const Stage& stage,const RoofSpan& roof) {
    return roof.kind<RoofKind::Count && roof.vertical<=1 && roof.length>=3 && roof.length<=9 &&
        roof.hp<=roof_health(roof.kind) && stage.in_bounds(roof.start) &&
        stage.in_bounds(roof_cell(roof,roof.length-1,2));
}
inline bool wooden_roof(const Stage& stage,Cell cell) {
    for (const RoofSpan& roof:stage.roofs)
        if (roof.kind==RoofKind::Log && roof_covers(roof,cell)) return true;
    return false;
}
void step_roofs(Game& game);
