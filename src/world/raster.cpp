#include "raster.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstdint>

namespace {
bool within(Cell p,int width,int height) {return p.x>=0 && p.y>=0 && p.x<width && p.y<height;}
bool inside(std::span<const Cell> vertices,Cell cell) {
    // Test tile centers against edge coordinates. Compare rationals with integer
    // products instead of floating point so all peers choose the same cells.
    const std::int64_t px=2LL*cell.x+1,py=2LL*cell.y+1;
    bool hit=false;
    for (std::size_t i=0,j=vertices.size()-1;i<vertices.size();j=i++) {
        const std::int64_t ax=2LL*vertices[i].x,ay=2LL*vertices[i].y;
        const std::int64_t bx=2LL*vertices[j].x,by=2LL*vertices[j].y;
        if ((ay>py)==(by>py)) continue;
        const auto left=(px-ax)*(by-ay),right=(bx-ax)*(py-ay);
        if ((by>ay && left<right) || (by<ay && left>right)) hit=!hit;
    }
    return hit;
}
bool append(RasterShape& result,Cell cell,std::size_t limit) {
    if (result.cells.size()>=limit) {result.truncated=true;return false;}
    result.cells.push_back(cell);return true;
}
}
RasterShape raster_polygon(std::span<const Cell> vertices,int width,int height,std::size_t limit) {
    RasterShape result;
    if (vertices.size()<3 || vertices.size()>64 || width<=0 || height<=0 || width>512 || height>512) return result;
    Cell low{width,height},high{};
    for (Cell p:vertices) {
        if (std::abs(static_cast<std::int64_t>(p.x))>4096 || std::abs(static_cast<std::int64_t>(p.y))>4096) {result.truncated=true;return result;}
        low.x=std::min(low.x,p.x);low.y=std::min(low.y,p.y);
        high.x=std::max(high.x,p.x);high.y=std::max(high.y,p.y);
    }
    low.x=std::max(0,low.x);low.y=std::max(0,low.y);
    high.x=std::min(width-1,high.x);high.y=std::min(height-1,high.y);
    for (int y=low.y;y<=high.y;++y) for (int x=low.x;x<=high.x;++x)
        if (inside(vertices,{x,y}) && !append(result,{x,y},limit)) return result;
    return result;
}
RasterShape raster_line(Cell a,Cell b,int radius,int width,int height,std::size_t limit) {
    RasterShape result;
    if (width<=0 || height<=0 || width>512 || height>512 || radius<0 || radius>32) return result;
    if (std::abs(static_cast<std::int64_t>(a.x))>4096 || std::abs(static_cast<std::int64_t>(a.y))>4096 ||
        std::abs(static_cast<std::int64_t>(b.x))>4096 || std::abs(static_cast<std::int64_t>(b.y))>4096) {result.truncated=true;return result;}
    std::vector<bool> marked(static_cast<std::size_t>(width*height),false);
    const int dx=std::abs(b.x-a.x),dy=-std::abs(b.y-a.y),sx=a.x<b.x ? 1 : -1,sy=a.y<b.y ? 1 : -1;
    int error=dx+dy;
    for (int step=0;step<8193;++step) {
        for (int y=-radius;y<=radius;++y) for (int x=-radius;x<=radius;++x) {
            const Cell cell=a+Cell{x,y};if (x*x+y*y>radius*radius || !within(cell,width,height)) continue;
            const auto index=static_cast<std::size_t>(cell.y*width+cell.x);
            if (!marked[index]) {if (!append(result,cell,limit)) return result;marked[index]=true;}
        }
        if (a==b) return result;
        const int twice=2*error;
        if (twice>=dy) {error+=dy;a.x+=sx;}
        if (twice<=dx) {error+=dx;a.y+=sy;}
    }
    result.truncated=true;return result;
}

std::vector<Cell> connected_raster(Cell start,std::span<const Cell> cells,
    std::span<const std::uint8_t> allowed,int width,int height) {
    if (width<=0 || height<=0 || width>512 || height>512 || allowed.size()!=static_cast<std::size_t>(width*height)) return {};
    std::vector<bool> proposed(allowed.size(),false),seen(allowed.size(),false);
    for (Cell cell:cells) if (within(cell,width,height)) proposed[static_cast<std::size_t>(cell.y*width+cell.x)]=true;
    std::vector<Cell> result,queue{start};
    for (std::size_t i=0;i<queue.size();++i) {
        const Cell cell=queue[i];if (!within(cell,width,height)) continue;
        const auto index=static_cast<std::size_t>(cell.y*width+cell.x);
        if (seen[index] || !proposed[index] || !allowed[index]) continue;
        seen[index]=true;result.push_back(cell);
        for (Cell d:{Cell{1,0},Cell{-1,0},Cell{0,1},Cell{0,-1}}) queue.push_back(cell+d);
    }
    return result;
}
