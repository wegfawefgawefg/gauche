#include "growth_paths.hpp"
#include <algorithm>

std::vector<GrowthPath> branching_paths(Game& game,Cell origin,Cell heading,GrowthSettings settings) {
    constexpr Cell directions[]{{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
    settings.branches=std::clamp(settings.branches,1,12);
    settings.segments=std::clamp(settings.segments,2,12);
    settings.min_step=std::clamp(settings.min_step,1,16);
    settings.max_step=std::clamp(settings.max_step,settings.min_step,24);
    int initial=0;
    for (int d=0;d<8;++d) if (directions[d]==heading) initial=d;
    std::vector<GrowthPath> paths;
    for (int n=0;n<settings.branches;++n) {
        GrowthPath path;int direction=initial;
        if (n==0) path.points.push_back(origin);
        else {
            path.parent=static_cast<int>(random_u32(game)%paths.size());
            const auto& parent=paths[static_cast<std::size_t>(path.parent)];
            const std::size_t joint=1+random_u32(game)%(parent.points.size()-1);
            path.points.push_back(parent.points[joint]);
            const Cell delta=parent.points[joint]-parent.points[joint-1];
            const Cell along{(delta.x>0)-(delta.x<0),(delta.y>0)-(delta.y<0)};
            for (int d=0;d<8;++d) if (directions[d]==along) direction=d;
            direction=(direction+(random_u32(game)%2 ? 2 : 6))%8;
        }
        const int segments=2+static_cast<int>(random_u32(game)%static_cast<unsigned>(settings.segments-1));
        for (int i=0;i<segments;++i) {
            // Persistence with small turns creates veins, not an isotropic random walk.
            const auto turn=random_u32(game)%5;
            direction=(direction+(turn==0 ? 7 : turn==1 ? 1 : 0))%8;
            const int step=settings.min_step+static_cast<int>(random_u32(game)%static_cast<unsigned>(settings.max_step-settings.min_step+1));
            const Cell d=directions[direction];
            path.points.push_back(path.points.back()+Cell{d.x*step,d.y*step});
        }
        paths.push_back(std::move(path));
    }
    return paths;
}
