#include "powder_monkey.hpp"
#include "../items/quarry_charge.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>
#include <array>
#include <vector>

namespace {
constexpr std::array<Cell,4> sides{{{1,0},{-1,0},{0,1},{0,-1}}};
struct Node { Cell cell{},first{}; int steps=0; };
std::vector<Node> local_routes(const Game& game,int slot) {
    const Cell origin=game.entities[static_cast<std::size_t>(slot)].cell;
    std::vector<Node> queue{{origin,origin,0}};
    std::vector<bool> seen(game.stage.tiles.size(),false);
    const auto index=[&](Cell cell) { return static_cast<std::size_t>(cell.y*game.stage.width+cell.x); };
    for (const auto& actor:game.entities)
        if (actor.kind!=EntityKind::None && actor.impassable && game.stage.in_bounds(actor.cell)) seen[index(actor.cell)]=true;
    seen[index(origin)]=true;
    for (std::size_t i=0;i<queue.size() && queue.size()<256;++i) {
        const Node node=queue[i];
        if (node.steps>=6) continue;
        for (Cell side:sides) {
            const Cell cell=node.cell+side;
            const Tile* tile=game.stage.at(cell);
            if (!tile || seen[index(cell)] || !walkable(*tile) || tile->kind==TileKind::Lava) continue;
            seen[index(cell)]=true;
            queue.push_back({cell,node.steps==0 ? cell : node.first,node.steps+1});
        }
    }
    return queue;
}
bool cover(const Game& game,Cell cell) {
    const Tile& tile=game.stage.at_or_border(cell);
    return quarry_wall(game.stage,cell) || (prop_blocks(tile.prop) && tile.prop.hp>0);
}
}
bool powder_danger(Cell cell,Cell charge,Cell direction) {
    const Cell delta=cell-charge;
    const int along=delta.x*direction.x+delta.y*direction.y;
    return delta.x*direction.y==delta.y*direction.x && along>=-1 && along<=3;
}
std::optional<PowderSite> powder_site(const Game& game,int slot,Cell threat) {
    std::optional<PowderSite> best;
    int score=-10000;
    for (const auto& node:local_routes(game,slot)) {
        if (surface_wet(game.stage.at_or_border(node.cell)) || distance(node.cell,threat)>6) continue;
        bool occupied=false;
        for (const auto& actor:game.entities)
            if (actor.kind==EntityKind::Projectile && actor.cell==node.cell) { occupied=true; break; }
        if (occupied) continue;
        for (Cell side:sides) {
            if (!cover(game,node.cell+side)) continue;
            // Prefer cover near the fight without re-planning every rendered frame.
            const int value=30-node.steps*3-distance(node.cell,threat)*2;
            if (value>score) { score=value; best=PowderSite{node.cell,side,node.first}; }
        }
    }
    return best;
}
std::optional<Cell> powder_escape(const Game& game,int slot,Cell charge,Cell direction) {
    std::optional<Cell> first; int score=-10000;
    for (const auto& node:local_routes(game,slot)) {
        if (powder_danger(node.cell,charge,direction)) continue;
        const int value=20+std::min(distance(node.cell,charge),4)*6-node.steps*4;
        if (value>score) { score=value; first=node.first; }
    }
    return first;
}
