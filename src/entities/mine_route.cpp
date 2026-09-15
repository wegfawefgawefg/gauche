#include "mine_crew.hpp"
#include <algorithm>
#include <limits>
#include <queue>
#include <vector>

bool crew_diggable(const Tile& tile) {
    return tile.kind==TileKind::Wall && tile.hp>0 && tile.break_rule!=BreakRule::Unbreakable &&
        (tile.break_rule!=BreakRule::DigRequired || tile.required_dig_power<=1);
}

std::optional<Cell> mine_route_step(const Game& game,int slot,Cell target) {
    const Entity& worker=game.entities[static_cast<std::size_t>(slot)];
    const Cell origin=worker.cell;
    if (!game.stage.in_bounds(target) || target==origin) return std::nullopt;
    const auto index=[&game](Cell cell) { return static_cast<std::size_t>(cell.y*game.stage.width+cell.x); };
    std::vector<int> cost(game.stage.tiles.size(),std::numeric_limits<int>::max());
    std::vector<bool> occupied(cost.size(),false);
    for (const Entity& actor:game.entities)
        if (actor.kind!=EntityKind::None && actor.impassable && actor.cell!=origin &&
            game.stage.in_bounds(actor.cell)) occupied[index(actor.cell)]=true;
    struct Node { int cost,serial; Cell cell,first; };
    const auto later=[](const Node& a,const Node& b) {
        return a.cost!=b.cost ? a.cost>b.cost : a.serial>b.serial;
    };
    std::priority_queue<Node,std::vector<Node>,decltype(later)> queue(later);
    queue.push({0,0,origin,origin}); cost[index(origin)]=0;
    constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
    int serial=1,visited=0;
    std::optional<Cell> fallback;
    int closest=distance(origin,target),best_cost=std::numeric_limits<int>::max();
    // COST: Walking takes a movement beat. Cutting adds each complete 36-tick
    // tell and 12-tick recovery needed at 25 damage, then the movement beat.
    // Stable tie order and a bounded expansion count keep replicas identical.
    while (!queue.empty() && visited<512) {
        const Node node=queue.top(); queue.pop();
        if (node.cost!=cost[index(node.cell)]) continue;
        ++visited;
        if (node.cell==target) return node.first;
        const int gap=distance(node.cell,target);
        if (gap<closest || (gap==closest && node.cost<best_cost && node.cell!=origin)) {
            closest=gap; best_cost=node.cost; fallback=node.first;
        }
        for (Cell side:sides) {
            const Cell cell=node.cell+side;
            const Tile* tile=game.stage.at(cell);
            if (!tile || tile->kind==TileKind::Lava || (occupied[index(cell)] && cell!=target)) continue;
            int step=std::max(1,worker.move_interval)*movement_slow_factor(worker);
            if (crew_diggable(*tile)) step+=((tile->hp+24)/25)*48;
            else if (!walkable(tile->kind)) continue;
            if (prop_blocks(tile->prop)) {
                if (tile->prop.hp==0) continue;
                step+=((tile->prop.hp+24)/25)*48;
            }
            if (occupied[index(cell)] && cell==target) {
                const int blocker=entity_at(game,cell,true);
                if (blocker>=0 && game.entities[static_cast<std::size_t>(blocker)].hard_blocker) continue;
            }
            const int total=node.cost+step;
            if (total>=cost[index(cell)]) continue;
            cost[index(cell)]=total;
            queue.push({total,serial++,cell,node.cell==origin ? cell : node.first});
        }
    }
    // Distant/blocked goals can still approach a useful frontier. Never return
    // the starting cell or an arbitrary more distant direction to spend a beat.
    return fallback;
}
