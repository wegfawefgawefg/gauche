#include "routed_path.hpp"
#include <algorithm>
#include <limits>
#include <queue>

std::vector<Cell> route_closed_cost_field(std::span<const Cell> guide,int width,int height,
                                         std::span<const unsigned> costs) {
    if (guide.size()<3 || width<=0 || height<=0 || width>512 || height>512 || costs.size()!=static_cast<std::size_t>(width*height)) return {};
    const auto index=[&](Cell cell){return static_cast<std::size_t>(cell.y*width+cell.x);};
    for (std::size_t i=0;i<guide.size();++i) {
        const Cell cell=guide[i];
        if (cell.x<0 || cell.y<0 || cell.x>=width || cell.y>=height || !costs[index(cell)]) return {};
        for (std::size_t j=0;j<i;++j) if (cell==guide[j]) return {};
    }
    std::vector<Cell> path{guide.front()};
    for (std::size_t i=0;i<guide.size();++i) {
        const Cell from=guide[i],to=guide[(i+1)%guide.size()];
        if (from==to) return {};
        std::vector<unsigned> available(costs.begin(),costs.end());
        for (Cell cell:path) available[index(cell)]=0;
        for (Cell cell:guide) available[index(cell)]=0;
        available[index(from)]=costs[index(from)];available[index(to)]=costs[index(to)];
        auto link=route_cost_field(from,to,width,height,available);
        if (link.size()<2) return {};
        path.insert(path.end(),link.begin()+1,link.end());
    }
    if (path.back()!=path.front()) return {};
    path.pop_back();return path;
}

std::vector<Cell> route_cost_field(Cell from,Cell to,int width,int height,std::span<const unsigned> costs) {
    const auto inside=[&](Cell c){return c.x>=0 && c.y>=0 && c.x<width && c.y<height;};
    if (width<=0 || height<=0 || width>512 || height>512 || costs.size()!=static_cast<std::size_t>(width*height) || !inside(from) || !inside(to)) return {};
    const int start=from.y*width+from.x,target=to.y*width+to.x;
    if (!costs[static_cast<std::size_t>(start)] || !costs[static_cast<std::size_t>(target)]) return {};
    using Node=std::pair<std::uint64_t,int>;
    std::priority_queue<Node,std::vector<Node>,std::greater<Node>> open;
    std::vector<std::uint64_t> best(costs.size(),std::numeric_limits<std::uint64_t>::max());
    std::vector<int> parent(costs.size(),-1);best[static_cast<std::size_t>(start)]=0;open.push({0,start});
    constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
    while (!open.empty()) {
        const auto [cost,index]=open.top();open.pop();
        if (cost!=best[static_cast<std::size_t>(index)]) continue;
        if (index==target) break;
        const Cell cell{index%width,index/width};
        for (Cell d:directions) {
            const Cell next=cell+d;if (!inside(next)) continue;
            const int id=next.y*width+next.x;const auto step=costs[static_cast<std::size_t>(id)];
            if (!step || cost+step>=best[static_cast<std::size_t>(id)]) continue;
            best[static_cast<std::size_t>(id)]=cost+step;parent[static_cast<std::size_t>(id)]=index;open.push({cost+step,id});
        }
    }
    if (best[static_cast<std::size_t>(target)]==std::numeric_limits<std::uint64_t>::max()) return {};
    std::vector<Cell> path;
    for (int index=target;;index=parent[static_cast<std::size_t>(index)]) {
        path.push_back({index%width,index/width});if (index==start) break;
    }
    std::reverse(path.begin(),path.end());return path;
}
