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

namespace {

constexpr Cell neighbors[]{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

bool free_cell(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    return tile != nullptr && walkable(*tile) && entity_at(game, cell, true) < 0;
}

bool boxed_in(const Game& game, Cell cell) {
    for (Cell direction : neighbors)
        if (free_cell(game, cell + direction)) return false;
    return true;
}

} // namespace

bool yield_trail(Game& game, int slot, const Entity& leader) {
    Entity& follower = game.entities[static_cast<std::size_t>(slot)];
    // CHAIN PRESSURE: A boxed-in leader requests space. Waiting children pass it
    // along until a tail can shuffle aside; no bird is pushed or teleported.
    if (distance(follower.cell, leader.cell) == 1 &&
        (leader.counter_c > 0 || boxed_in(game, leader.cell))) follower.counter_c = 45;
    if (follower.counter_c == 0) return false;
    if (follower.move_wait > 0) return true;
    const int start = static_cast<int>(random_u32(game) % 4);
    for (int offset = 0; offset < 4; ++offset) {
        const Cell destination = follower.cell + neighbors[(start + offset) % 4];
        if (distance(destination, leader.cell) <= 1 || !free_cell(game, destination)) continue;
        if (move_entity(game, slot, destination)) follower.counter_c = 0;
        return true;
    }
    follower.move_wait = 2;
    return true;
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
