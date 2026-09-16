#include "bear_clearings.hpp"
#include "room_supplies.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../entities/bear_family.hpp"
#include "../props/interaction.hpp"
#include "../items/supply.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

void populate_bear_beds(Game& game,std::span<const Cell> beds,bool family) {
    Handle parent{};
    for (std::size_t i=0;i<beds.size();++i) {
        const auto handle=spawn_entity(game,EntityKind::Bear,beds[i]);
        auto* bear=get_entity(game,handle);
        if (!bear) continue;
        place_prop(game.stage,beds[i],PropKind::BearBed,static_cast<std::uint8_t>(random_u32(game)));
        if (family) {
            if (parent.slot<0) parent=handle;
            set_bear_role(*bear,i==0 ? BearMother : i==1 ? BearFather : BearCub);
            bear->entity_a=parent;
        }
        apply_sleep(*bear,36000);
    }
}

std::vector<std::size_t> populate_bear_clearings(Game& game,const FloorPlan& plan,PopulationReport* report) {
    std::vector<std::size_t> claimed;
    if (!forest_floor(game.run.floor) || random_u32(game)%(game.run.floor==1 ? 3U : 2U)!=0) return claimed;
    std::vector<std::size_t> rooms;
    for (std::size_t i=0;i<plan.rooms.size();++i) {
        const auto& room=plan.rooms[i];
        if ((room.role==RoomRole::Den || room.role==RoomRole::Clearing || room.role==RoomRole::Thicket) &&
            !reserved_habitat(room) && static_cast<int>(i)!=plan.objective_room) rooms.push_back(i);
    }
    for (std::size_t i=rooms.size();i>1;--i) std::swap(rooms[i-1],rooms[random_u32(game)%i]);
    // Independent habitat allowance: a filled scene replaces that room's generic
    // encounter, not its terrain. Actual dry sockets retain each room's outline.
    const unsigned kind=random_u32(game)%4; // lone elder, roaming adult, two family rolls
    for (auto index:rooms) {
        auto sockets=room_spaces(game,plan.rooms[index]);
        std::erase_if(sockets,[&](Cell c) {
            const auto& tile=game.stage.at_or_border(c);
            return plan.protected_cell(c) || surface_wet(tile) || tile.prop.kind!=PropKind::None ||
                distance(c,game.run.spawn)<10;
        });
        if (sockets.size()<12) continue;
        for (std::size_t i=sockets.size();i>1;--i) std::swap(sockets[i-1],sockets[random_u32(game)%i]);
        const Cell anchor=sockets.front();
        std::vector<Cell> beds;
        const std::size_t wanted=kind<2 ? 1U : 3+random_u32(game)%4;
        for (Cell c:sockets) {
            if (distance(c,anchor)>5) continue;
            bool near=false;for (Cell bed:beds) if (distance(c,bed)<2) near=true;
            if (near) continue;
            beds.push_back(c);if (beds.size()==wanted) break;
        }
        if (beds.size()!=wanted) continue;
        if (kind>=2) populate_bear_beds(game,beds,true);
        else if (auto* bear=get_entity(game,spawn_entity(game,EntityKind::Bear,beds[0]))) {
            set_bear_role(*bear,kind==0 ? BearOld : BearOrdinary);
            if (kind==0) {
                place_prop(game.stage,beds[0],PropKind::BearBed);
                if (random_u32(game)%2==0) apply_sleep(*bear,36000);
            }
        } else continue;
        claimed.push_back(index);
        if (report) report->bear_clearings.push_back({index,kind,beds});
        // Child sockets: rolled cache, food and remains, with independent counts.
        std::erase_if(sockets,[&](Cell c){return entity_at(game,c,false)>=0;});
        if (!sockets.empty() && (kind==0 || random_u32(game)%2==0)) {
            const auto tool=roll_item_supply(game,LootSource::Weapon,false);
            place_ground_item(game,sockets.back(),tool,supply_count(tool));sockets.pop_back();
            if (!sockets.empty()) {
                place_coins(game,sockets.back(),kind==0 ? 30 : 12);sockets.pop_back();
            }
        }
        if (!sockets.empty() && random_u32(game)%3==0) {
            place_ground_item(game,sockets.back(),random_u32(game)%2 ? ItemKind::RawMeat : ItemKind::HoneyPot);
            sockets.pop_back();
        }
        const int remains=3+static_cast<int>(random_u32(game)%(kind==0 ? 10U : 6U));
        for (int i=0;i<remains && !sockets.empty();++i) {
            const Cell c=sockets.back();sockets.pop_back();
            place_prop(game.stage,c,random_u32(game)%4 ? PropKind::BonePile : PropKind::RottenLog,
                static_cast<std::uint8_t>(random_u32(game)));
        }
        break;
    }
    return claimed;
}
