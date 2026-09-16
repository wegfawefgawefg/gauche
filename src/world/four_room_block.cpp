#include "four_room_block.hpp"
#include <algorithm>

namespace {
int room_at(const FloorPlan& plan,Cell grid) {
    for (int i=0;i<static_cast<int>(plan.rooms.size());++i) if (plan.rooms[static_cast<std::size_t>(i)].grid==grid) return i;
    return -1;
}
bool eligible(const FloorPlan& plan,int i) {
    return i!=0 && i!=plan.exit_room && i!=plan.objective_room && i!=plan.secret_room &&
        !reserved_habitat(plan.rooms[static_cast<std::size_t>(i)]);
}
}

std::optional<Cell> reserve_four_rooms(Game& game,FloorPlan& plan,std::array<int,4>& rooms) {
    Cell low{7,7},high{};
    for (const auto& room:plan.rooms) {
        low.x=std::min(low.x,room.grid.x);low.y=std::min(low.y,room.grid.y);
        high.x=std::max(high.x,room.grid.x);high.y=std::max(high.y,room.grid.y);
    }
    std::vector<Cell> choices;
    for (int y=low.y;y<high.y;++y) for (int x=low.x;x<high.x;++x) {
        int present=0;bool clear=true;
        for (Cell d:{Cell{0,0},Cell{1,0},Cell{0,1},Cell{1,1}}) {
            const int i=room_at(plan,Cell{x,y}+d);if (i<0) continue;
            ++present;if (!eligible(plan,i)) clear=false;
        }
        if (clear && present>=2) for (int weight=0;weight<present;++weight) choices.push_back({x,y});
    }
    if (choices.empty()) return std::nullopt;
    const Cell grid=choices[random_u32(game)%choices.size()];
    int index=0;
    // Reserve four real planning cells. Missing corners become connected rooms
    // within existing world bounds; no objective or other landmark is displaced.
    for (Cell d:{Cell{0,0},Cell{1,0},Cell{0,1},Cell{1,1}}) {
        const Cell at_grid=grid+d;int i=room_at(plan,at_grid);
        if (i<0) {
            RoomPlan room;room.grid=at_grid;room.center={(at_grid.x-low.x)*22+12,(at_grid.y-low.y)*22+12};
            room.half_width=room.half_height=8;room.depth=plan.rooms.back().depth+1;
            i=static_cast<int>(plan.rooms.size());plan.rooms.push_back(room);
        }
        rooms[static_cast<std::size_t>(index++)]=i;
        auto& room=plan.rooms[static_cast<std::size_t>(i)];room.landmark=true;room.role=RoomRole::Den;
    }
    for (int a:rooms) for (int b:rooms) if (a<b && distance(plan.rooms[static_cast<std::size_t>(a)].grid,plan.rooms[static_cast<std::size_t>(b)].grid)==1) {
        bool connected=false;for (auto edge:plan.edges) if ((edge.a==a&&edge.b==b)||(edge.a==b&&edge.b==a)) connected=true;
        if (!connected) plan.edges.push_back({a,b});
    }
    return Cell{(grid.x-low.x)*22+23,(grid.y-low.y)*22+23};
}
