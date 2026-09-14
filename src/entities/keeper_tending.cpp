#include "candle_keeper.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../props/candle.hpp"
#include "../surfaces/interaction.hpp"

#include <array>
#include <vector>

// TERRITORY: One bounded flood finds reachable wicks, without crossing closed doors.
std::optional<Cell> keeper_candle(const Game& game, int slot) {
    const Entity& keeper = game.entities[static_cast<std::size_t>(slot)];
    constexpr int radius = 9, side = radius*2+1;
    const Cell home = keeper.point_a;
    if (distance(home,keeper.cell) > radius) return std::nullopt;
    const auto index = [home](Cell cell) {
        return static_cast<std::size_t>((cell.y-home.y+radius)*side+cell.x-home.x+radius);
    };
    std::array<bool,side*side> seen{}, blocked{};
    for (const Entity& actor : game.entities)
        if (actor.kind != EntityKind::None && actor.impassable && actor.cell != keeper.cell &&
            distance(home,actor.cell) <= radius) blocked[index(actor.cell)] = true;
    std::vector<Cell> queue{keeper.cell};
    seen[index(keeper.cell)] = true;
    std::optional<Cell> lit;
    for (std::size_t next=0;next<queue.size();++next) {
        const Cell cell = queue[next];
        const Tile* tile = game.stage.at(cell);
        if (!tile) continue;
        if (distance(home,cell) <= 8 && tile->prop.kind == PropKind::Candle &&
            !tile->prop.broken && tile->prop.growth_ticks > 0 && !surface_wet(*tile)) {
            if (!candle_lit(tile->prop)) return cell;
            if (!lit) lit = cell;
        }
        for (Cell offset : {Cell{1,0},{0,1},{-1,0},{0,-1}}) {
            const Cell cell_next = cell+offset;
            const Tile* neighbor = game.stage.at(cell_next);
            if (!neighbor || distance(home,cell_next) > radius) continue;
            const auto at = index(cell_next);
            if (seen[at] || blocked[at] || !walkable(*neighbor)) continue;
            seen[at] = true; queue.push_back(cell_next);
        }
    }
    return lit;
}

void keeper_walk(Game& game, int slot, Cell target) {
    Entity& keeper = game.entities[static_cast<std::size_t>(slot)];
    if (keeper.move_wait > 0 || keeper.cell == target) return;
    const auto next = next_route_cell(game,slot,target,240);
    if (next && (distance(keeper.point_a,*next) <= 9 ||
        distance(keeper.point_a,*next) < distance(keeper.point_a,keeper.cell)))
        willing_step(game,slot,*next);
    else keeper.move_wait = keeper.move_interval;
}

// WITNESS: No omniscient aggro when someone pockets a candle behind a closed door.
void keeper_candle_stolen(Game& game, Cell cell, Handle thief) {
    const Entity* taker = get_entity(game,thief);
    if (!taker || taker->health <= 0) return;
    for (Entity& keeper : game.entities) {
        if (keeper.kind != EntityKind::CandleKeeper || keeper.health <= 0 || keeper.sleep_ticks > 0 ||
            distance(keeper.point_a,cell) > 8 || distance(keeper.cell,cell) > 7 ||
            !clear_sight(game,keeper.cell,cell)) continue;
        keeper.entity_b = thief; keeper.attack_wait = 300;
        emit_sound(game,SoundId::KeeperScold,keeper.cell);
    }
}
