#pragma once
#include "roof.hpp"

inline bool tree_ellipse(const RoofSpan& roof,Cell cell,int inset=0) {
    const int rx=roof.length/2-inset,ry=roof.width/2-inset;
    if (rx<=0 || ry<=0) return false;
    const int x=cell.x-roof.start.x-roof.length/2,y=cell.y-roof.start.y-roof.width/2;
    return x*x*ry*ry+y*y*rx*rx<=rx*rx*ry*ry;
}
void step_hollow_tree(Game& game,RoofSpan& roof);
