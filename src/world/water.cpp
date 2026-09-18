#include "../items/basic_actions.hpp"
#include "water.hpp"
#include "currents.hpp"
#include "../entities/bell_diver.hpp"
#include "route.hpp"

#include <cstdlib>
#include <algorithm>

bool shallow_water(TileKind kind) {
    return kind == TileKind::ShallowWater || kind == TileKind::Spring || kind == TileKind::IceHole;
}

bool supports_wall_spring(const Stage& stage,Cell cell) {
    // Later carving may open the surrounding sector, but the source needs a wall face.
    if(stage.at_or_border(cell).kind!=TileKind::Wall)return false;
    constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
    for(unsigned i=0;i<4;++i) {
        const auto& tile=stage.at_or_border(cell+directions[i]);
        if(tile.kind==TileKind::Spring && stored_current_direction(tile)==directions[i])return true;
    }
    return false;
}

bool wading_actor(const Entity& actor) {
    if (basic_airborne(actor) || actor.toss.ticks>0 || actor.move_interval <= 0 || actor.health <= 0 || diver_submerged(actor)) return false;
    switch (actor.kind) {
    case EntityKind::Wasp: case EntityKind::CarrionCrow:
    case EntityKind::Mosquito: case EntityKind::Owl: case EntityKind::Woodpecker:
    case EntityKind::FurnaceMoth: case EntityKind::Bat: case EntityKind::FrostBat: case EntityKind::LanternMoth:
    case EntityKind::Train: case EntityKind::RailLayer:
        return false;
    default: return true;
    }
}

void place_water_scenes(Game& game, const FloorPlan& plan) {
    if (!forest_floor(game.run.floor)) return;
    for (const RoomPlan& room : plan.rooms) {
        if (room.role != RoomRole::Brook) continue;
        struct Source {Cell cell,flow;};
        std::vector<Source> choices;
        constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
        for (int y=-room.half_height;y<=room.half_height;++y)
            for (int x=-room.half_width;x<=room.half_width;++x) {
                const Cell cell=room.center+Cell{x,y};
                const Tile* tile=game.stage.at(cell);
                if (!tile || !walkable(*tile) || plan.protected_cell(cell)) continue;
                for (Cell flow:directions) {
                    const Tile& next=game.stage.at_or_border(cell+flow);
                    if (game.stage.at_or_border(cell-flow).kind==TileKind::Wall &&
                        walkable(next) && !plan.protected_cell(cell+flow)) choices.push_back({cell,flow});
                }
            }
        if (choices.empty()) continue;
        const auto source=choices[random_u32(game)%choices.size()];
        const Cell across{-source.flow.y,source.flow.x};
        const int reach=3+static_cast<int>(random_u32(game)%3);
        const int width=1+static_cast<int>(random_u32(game)%2);
        const auto code=static_cast<std::uint8_t>(source.flow.x>0 ? 1 : source.flow.y>0 ? 2 : source.flow.x<0 ? 3 : 4);
        for (int along=0;along<=reach;++along) {
            const int radius=along==0 ? 0 : along==reach ? 1 : width;
            for (int side=-radius;side<=radius;++side) {
                const Cell cell=source.cell+Cell{source.flow.x*along+across.x*side,source.flow.y*along+across.y*side};
                Tile* tile=game.stage.at(cell);
                if (!tile || !walkable(*tile) || plan.protected_cell(cell) ||
                    std::abs(cell.x-room.center.x)>room.half_width ||
                    std::abs(cell.y-room.center.y)>room.half_height) continue;
                *tile={TileKind::ShallowWater};tile->current=code;
            }
        }
        *game.stage.at(source.cell)={TileKind::Spring};
        game.stage.at(source.cell)->current=code;
    }
}
