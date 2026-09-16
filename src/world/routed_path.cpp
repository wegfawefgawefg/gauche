#include "routed_path.hpp"
#include <algorithm>
#include <limits>
#include <queue>

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
