#include "following.hpp"
#include "behavior.hpp"

void record_trail(Entity& leader, Cell previous, int delay) {
    if (leader.cell == previous) return;
    leader.point_b = previous;
    leader.timer_b = delay;
}

void follow_trail(Game& game, int slot, const Entity& leader) {
    Entity& follower = game.entities[static_cast<std::size_t>(slot)];
    if (leader.timer_b > 0 || follower.move_wait > 0 || follower.cell == leader.point_b) return;
    // REST: A stationary leader has no departed cell yet.
    if (leader.point_b == leader.cell && distance(follower.cell, leader.cell) <= 1) return;
    pursue(game, slot, leader.point_b);
}

Handle find_chain_tail(const Game& game, int slot, EntityKind kind, int radius) {
    const Entity& follower = game.entities[static_cast<std::size_t>(slot)];
    Handle best;
    // ORDER: Reattachment only points toward older slots, so repairs cannot form cycles.
    for (int i = 0; i < slot; ++i) {
        const Entity& candidate = game.entities[static_cast<std::size_t>(i)];
        if (candidate.kind != kind || candidate.health <= 0) continue;
        const int gap = distance(follower.cell, candidate.cell);
        if (gap > radius) continue;
        const Handle handle{i, candidate.generation};
        bool occupied = false;
        for (int child = 0; child < max_entities; ++child) {
            const Entity& member = game.entities[static_cast<std::size_t>(child)];
            if (child != slot && member.kind == kind && member.health > 0 && member.entity_a == handle) {
                occupied = true;
                break;
            }
        }
        if (!occupied) { best = handle; radius = gap; }
    }
    return best;
}
