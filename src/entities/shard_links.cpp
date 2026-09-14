#include "shard_colony.hpp"
#include "attacks.hpp"
#include "../world/ground_items.hpp"

#include <cstdlib>

ShardGroup shard_group(const Game& game, Handle colony) {
    ShardGroup group;
    for (int slot=0;slot<max_entities && group.count<3;++slot) {
        const Entity& node = game.entities[static_cast<std::size_t>(slot)];
        if (node.kind == EntityKind::ShardColony && node.health > 0 && node.entity_a == colony)
            group.nodes[static_cast<std::size_t>(group.count++)] = {slot,node.generation};
    }
    return group;
}

namespace {

bool endpoint(const Entity& node) {
    return node.health > 0 && node.timer_b == 0 && node.sleep_ticks == 0 && node.stun_ticks == 0 &&
        node.vitals.rooted == 0 && node.cell == node.point_a &&
        (node.label_a == ShardCharge || node.label_a == ShardPulse);
}

std::vector<Cell> trace(Cell from, Cell to) {
    std::vector<Cell> cells;
    const int dx = std::abs(to.x-from.x), dy = -std::abs(to.y-from.y);
    const int sx = from.x<to.x ? 1 : -1, sy = from.y<to.y ? 1 : -1;
    int error = dx+dy;
    while (true) {
        cells.push_back(from);
        if (from == to) break;
        const int twice = error*2;
        if (twice >= dy) { error += dy; from.x += sx; }
        if (twice <= dx) { error += dx; from.y += sy; }
    }
    return cells;
}

} // namespace

std::vector<ShardLink> shard_links(const Game& game, const ShardGroup& group) {
    std::vector<ShardLink> links;
    for (int a=0;a<group.count;++a) {
        const Entity* first = get_entity(game,group.nodes[static_cast<std::size_t>(a)]);
        if (!first || !endpoint(*first)) continue;
        for (int b=a+1;b<group.count;++b) {
            const Entity* second = get_entity(game,group.nodes[static_cast<std::size_t>(b)]);
            if (!second || !endpoint(*second) || distance(first->cell,second->cell) > 8 ||
                first->cell == second->cell || !clear_attack_sight(game,first->cell,second->cell,false)) continue;
            links.push_back({first->cell,second->cell,trace(first->cell,second->cell)});
        }
    }
    return links;
}

// REWARD: The colony identity stays valid as a grouping key after its first node dies.
// Slot reuse cannot join another colony because its generation is part of the key.
void drop_shard_colony(Game& game, const Entity& node) {
    if (shard_group(game,node.entity_a).count == 0 && random_u32(game)%4 == 0)
        place_ground_item(game,node.cell,ItemKind::CrystalLens);
}
