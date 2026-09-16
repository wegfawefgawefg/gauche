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
