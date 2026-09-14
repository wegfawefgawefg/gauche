#include "seal_thief.hpp"
#include "../surfaces/interaction.hpp"

#include <algorithm>
#include <array>

namespace {

constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
constexpr int radius = 8, width = radius * 2 + 1;
struct Node { Cell cell, first; int steps; };

bool open(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    return tile && walkable(*tile) && tile->kind != TileKind::Lava;
}

} // namespace

bool seal_food(ItemKind kind) {
    return kind == ItemKind::SaltedKelp || kind == ItemKind::SmokedFish || kind == ItemKind::RawMeat ||
        kind == ItemKind::CookedMeat || kind == ItemKind::Egg || kind == ItemKind::FriedEgg;
}

bool seal_bank(const Game& game, Cell cell) {
    if (!open(game,cell)) return false;
    for (Cell side : sides) {
        const Tile* tile = game.stage.at(cell+side);
        if (tile && (tile->kind == TileKind::Water || surface_wet(*tile))) return true;
    }
    return false;
}

// SEARCH: One bounded flood builds occupancy once, for both food and safe banks.
SealRoute seal_route(const Game& game, int slot, bool food, std::optional<Cell> threat) {
    const Cell origin = game.entities[static_cast<std::size_t>(slot)].cell;
    const auto index = [origin](Cell cell) {
        return static_cast<std::size_t>((cell.y-origin.y+radius)*width+cell.x-origin.x+radius);
    };
    std::array<bool,width*width> seen{};
    for (const Entity& actor : game.entities)
        if (actor.kind != EntityKind::None && actor.impassable && actor.cell != origin &&
            distance(origin,actor.cell) <= radius) seen[index(actor.cell)] = true;
    std::array<Node,width*width> nodes{};
    nodes[0] = {origin,origin,0};
    seen[index(origin)] = true;
    int count = 1;
    for (int next=0;next<count;++next) {
        const Node node = nodes[static_cast<std::size_t>(next)];
        if (node.steps >= radius) continue;
        for (Cell side : sides) {
            const Cell cell = node.cell+side;
            if (distance(origin,cell) > radius || seen[index(cell)] || !open(game,cell)) continue;
            seen[index(cell)] = true;
            nodes[static_cast<std::size_t>(count++)] = {cell,next == 0 ? cell : node.first,node.steps+1};
        }
    }
    SealRoute route;
    int best = -1000;
    if (food) {
        for (int i=0;i<max_entities;++i) {
            const Entity& loose = game.entities[static_cast<std::size_t>(i)];
            if (loose.kind != EntityKind::GroundItem || !seal_food(loose.ground_item.kind) ||
                loose.ground_item.count <= 0 || loose.ground_item.flame_ticks > 0 ||
                distance(origin,loose.cell) > radius) continue;
            for (int n=0;n<count;++n) {
                const Node node = nodes[static_cast<std::size_t>(n)];
                if (node.cell != loose.cell) continue;
                const int score = (loose.ground_item.kind == ItemKind::SmokedFish ? 16 : 0)-node.steps;
                if (score > best) {
                    best = score; route = {true,node.first,node.cell,{i,loose.generation}};
                }
                break;
            }
        }
        return route;
    }
    for (int n=0;n<count;++n) {
        const Node node = nodes[static_cast<std::size_t>(n)];
        if (!seal_bank(game,node.cell)) continue;
        const int score = (threat ? std::min(5,distance(node.cell,*threat))*32 : 0)-node.steps;
        if (score > best) { best = score; route = {true,node.first,node.cell,{}}; }
    }
    return route;
}
