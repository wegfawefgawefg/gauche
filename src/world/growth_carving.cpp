#include "growth_carving.hpp"

bool generation_lock_intact(const Game& game,const FloorPlan& plan) {
    const Cell start=plan.rooms.front().center,exit=plan.rooms[static_cast<std::size_t>(plan.exit_room)].center;
    const Cell objective=plan.rooms[static_cast<std::size_t>(plan.objective_room)].center;
    std::vector<bool> visited(game.stage.tiles.size(),false);std::vector<Cell> queue{start};
    bool reached=false;
    constexpr Cell directions[]{{1,0},{-1,0},{0,1},{0,-1}};
    for (std::size_t i=0;i<queue.size();++i) {
        const Cell cell=queue[i];const Tile* tile=game.stage.at(cell);
        if (!tile || !walkable(*tile) || cell==plan.door) continue;
        const auto index=static_cast<std::size_t>(cell.y*game.stage.width+cell.x);
        if (visited[index]) continue;
        visited[index]=true;
        if (cell==exit) return false;
        if (cell==objective) reached=true;
        for (Cell direction:directions) queue.push_back(cell+direction);
    }
    return reached;
}

namespace {
std::vector<int> walking_parents(const Game& game,Cell start,Cell blocked) {
    std::vector<int> parents(game.stage.tiles.size(),-1);
    if (!game.stage.in_bounds(start) || !walkable(game.stage.at_or_border(start)) || start==blocked) return parents;
    const int first=start.y*game.stage.width+start.x;
    parents[static_cast<std::size_t>(first)]=first;
    std::vector<Cell> queue{start};
    for (std::size_t i=0;i<queue.size();++i) {
        for (Cell d:{Cell{1,0},Cell{-1,0},Cell{0,1},Cell{0,-1}}) {
            const Cell cell=queue[i]+d;const auto* tile=game.stage.at(cell);
            if (!tile || !walkable(*tile) || cell==blocked) continue;
            const auto index=static_cast<std::size_t>(cell.y*game.stage.width+cell.x);
            if (parents[index]>=0) continue;
            parents[index]=queue[i].y*game.stage.width+queue[i].x;queue.push_back(cell);
        }
    }
    return parents;
}
}
bool generation_exit_reachable(const Game& game,const FloorPlan& plan) {
    const auto parents=walking_parents(game,plan.rooms.front().center,{-1,-1});
    const Cell exit=plan.rooms[static_cast<std::size_t>(plan.exit_room)].center;
    return game.stage.in_bounds(exit) && parents[static_cast<std::size_t>(exit.y*plan.width+exit.x)]>=0;
}
std::vector<std::uint8_t> generation_walking_routes(const Game& game,const FloorPlan& plan) {
    std::vector<std::uint8_t> mask(game.stage.tiles.size(),0);
    const auto mark=[&](const std::vector<int>& parents,Cell end) {
        if (!game.stage.in_bounds(end)) return;
        int index=end.y*plan.width+end.x;
        if (parents[static_cast<std::size_t>(index)]<0) return;
        while (!mask[static_cast<std::size_t>(index)]) {
            mask[static_cast<std::size_t>(index)]=1;
            const int previous=parents[static_cast<std::size_t>(index)];
            if (previous==index) break;
            index=previous;
        }
    };
    const auto open=walking_parents(game,plan.rooms.front().center,{-1,-1});
    for (const auto& room:plan.rooms) mark(open,room.center);
    for (const auto& tree:plan.giant_trees) {
        mark(open,tree.cache);for (Cell mouth:tree.entrances) mark(open,mouth);
    }
    for (const auto& tunnel:plan.snake_tunnels) {mark(open,tunnel.entry);mark(open,tunnel.cache);}
    const auto shut=walking_parents(game,plan.rooms.front().center,plan.door);
    // This second route must not stop at a cell marked by a different BFS tree.
    const Cell objective=plan.rooms[static_cast<std::size_t>(plan.objective_room)].center;
    if (game.stage.in_bounds(objective)) {
        int index=objective.y*plan.width+objective.x;
        while (shut[static_cast<std::size_t>(index)]>=0) {
            mask[static_cast<std::size_t>(index)]=1;
            const int previous=shut[static_cast<std::size_t>(index)];if (previous==index) break;index=previous;
        }
    }
    return mask;
}
