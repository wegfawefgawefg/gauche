#include "bear_stream.hpp"
#include "population_report.hpp"
#include "ground_items.hpp"
#include "water.hpp"
#include "../entities/bear_fishing.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

namespace {
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
struct Bank {Cell stand,fish;};
bool empty_bank(const Game& game,const FloorPlan& plan,Cell cell) {
    const auto& tile=game.stage.at_or_border(cell);
    return walkable(tile) && !surface_wet(tile) && tile.kind!=TileKind::Lava &&
        tile.prop.kind==PropKind::None && !plan.protected_cell(cell) &&
        entity_at(game,cell,false)<0 && distance(cell,game.run.spawn)>=8;
}
}

// Habitat component: choose an existing wet/dry edge, then roll its inhabitants,
// remains and equipment independently. Neither room corners nor facing are fixed.
void populate_bear_streams(Game& game,const FloorPlan& plan,PopulationReport* report) {
    if (!forest_floor(game.run.floor)) return;
    int sites=0;
    for (const auto& room:plan.rooms) {
        if (room.role!=RoomRole::Brook || sites>=2 || random_u32(game)%3==0) continue;
        std::vector<Bank> banks;
        for (int y=-room.half_height;y<=room.half_height;++y)
            for (int x=-room.half_width;x<=room.half_width;++x) {
                const Cell fish=room.center+Cell{x,y};
                if (!shallow_water(game.stage.at_or_border(fish).kind) ||
                    plan.protected_cell(fish) || entity_at(game,fish,false)>=0) continue;
                for (Cell side:sides) {
                    const Cell stand=fish+side;
                    if (!empty_bank(game,plan,stand)) continue;
                    int exits=0;
                    for (Cell approach:sides) {
                        const auto& tile=game.stage.at_or_border(stand+approach);
                        exits+=walkable(tile) && !surface_wet(tile);
                    }
                    if (exits>=2) banks.push_back({stand,fish});
                }
            }
        if (banks.empty()) continue;
        const Bank bank=banks[random_u32(game)%banks.size()];
        const Handle handle=spawn_entity(game,EntityKind::Bear,bank.stand);
        Entity* bear=get_entity(game,handle);
        if (!bear) continue;
        const int stock=3+static_cast<int>(random_u32(game)%4);
        if (!get_entity(game,place_ground_item(game,bank.fish,ItemKind::RiverFish,stock))) {
            remove_entity(game,handle);continue;
        }
        start_bear_fishing(*bear);bear->facing=bank.fish-bank.stand;
        ++sites;
        if (report) report->bear_fishers.push_back(bank.stand);
        // Scattered optional sockets follow the selected bank, not a prefab row.
        std::vector<Cell> sockets;
        for (int y=-3;y<=3;++y) for (int x=-3;x<=3;++x) {
            const Cell cell=bank.stand+Cell{x,y};
            if (empty_bank(game,plan,cell) && distance(cell,bank.stand)>1) sockets.push_back(cell);
        }
        for (std::size_t i=sockets.size();i>1;--i)
            std::swap(sockets[i-1],sockets[random_u32(game)%i]);
        if (!sockets.empty() && random_u32(game)%3==0) {
            place_ground_item(game,sockets.back(),ItemKind::FishingLine);
            sockets.pop_back();
        }
        const int pieces=2+static_cast<int>(random_u32(game)%6);
        for (int i=0;i<pieces && !sockets.empty();++i) {
            const unsigned roll=random_u32(game)%5;
            const auto prop=roll<2 ? PropKind::BonePile : roll<4 ? PropKind::Fern : PropKind::RottenLog;
            place_prop(game.stage,sockets.back(),prop,static_cast<std::uint8_t>(random_u32(game)));
            sockets.pop_back();
        }
    }
}
