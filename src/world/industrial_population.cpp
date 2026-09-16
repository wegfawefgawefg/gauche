#include "freight_siding.hpp"
#include "settling_tanks.hpp"
#include "casting_floor.hpp"
#include "room_supplies.hpp"
#include "hoist_shaft.hpp"
#include "ash_loft.hpp"
#include "slag_bank.hpp"
#include "lamp_alcove.hpp"
#include "pay_office.hpp"
#include "kiln_court.hpp"
#include "cable_trench.hpp"
#include "cooling_works.hpp"
#include "scrap_yard.hpp"
#include "repair_bay.hpp"
#include "workfront.hpp"
#include "rivet_post.hpp"
#include "assembly.hpp"
#include "industrial_population.hpp"
#include "ground_items.hpp"
#include <algorithm>

namespace {
bool available(RoomSupplies& budget,RoomRole role,int threat,int gear=0) {
    const bool okay=budget.threat>=threat && budget.equipment>=gear;
    if (!okay && budget.report) ++budget.report->scenes[static_cast<std::size_t>(role)].budget_blocked;
    return okay;
}
bool result(RoomSupplies& budget,RoomRole role,bool placed) {
    if (budget.report) {
        auto& count=budget.report->scenes[static_cast<std::size_t>(role)];
        ++count.attempted;if (placed) ++count.placed;else ++count.rejected;
    }
    return placed;
}
void fallback(Game& game,const RoomPlan& room,EntityKind kind,int cost,RoomSupplies& budget) {
    const auto handle=spawn_room_enemy(game,room,kind,cost,budget);
    if (get_entity(game,handle) && budget.report) ++budget.report->scenes[static_cast<std::size_t>(room.role)].fallbacks;
}
}

void populate_industrial_rooms(Game& game,const FloorPlan& plan,RoomSupplies& budget) {
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::FreightSiding && available(budget,room.role,3,2)) {
        if (result(budget,room.role,populate_freight_siding(game,plan,room))) {budget.threat-=3;budget.equipment-=2;}
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::SettlingTanks && available(budget,room.role,3,2)) {
        if (result(budget,room.role,populate_settling_tanks(game,plan,room))) {budget.threat-=3;budget.equipment-=2;}
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::CastingFloor && available(budget,room.role,2,2)) {
        if (result(budget,room.role,populate_casting_floor(game,plan,room))) {budget.threat-=2;budget.equipment-=2;}
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::HoistShaft && available(budget,room.role,3,2)) {
        if (result(budget,room.role,populate_hoist_shaft(game,plan,room))) {budget.threat-=3;budget.equipment-=2;}
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::AshLoft && available(budget,room.role,2,2)) {
        if (result(budget,room.role,populate_ash_loft(game,plan,room))) {budget.threat-=2;budget.equipment-=2;}
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::SlagBank && available(budget,room.role,2,1)) {
        if (result(budget,room.role,populate_slag_bank(game,plan,room))) {budget.threat-=2;--budget.equipment;}
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::LampAlcove && available(budget,room.role,2,1)) {
        if (result(budget,room.role,populate_lamp_alcove(game,plan,room))) {budget.threat-=2;--budget.equipment;}
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::PayOffice && available(budget,room.role,3)) {
        if (result(budget,room.role,populate_pay_office(game,plan,room))) budget.threat-=3;
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::KilnCourt && available(budget,room.role,3)) {
        if (result(budget,room.role,populate_kiln_court(game,plan,room))) {budget.threat-=3;budget.equipment=std::max(0,budget.equipment-1);}
        else fallback(game,room,EntityKind::WalkingKiln,3,budget);
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::CableTrench && available(budget,room.role,2)) {
        if (result(budget,room.role,populate_cable_trench(game,plan,room))) {budget.threat-=2;budget.equipment=std::max(0,budget.equipment-1);}
        else fallback(game,room,EntityKind::CableCrawler,2,budget);
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::CoolingWorks && available(budget,room.role,3)) {
        if (result(budget,room.role,populate_cooling_works(game,plan,room))) {budget.threat-=3;budget.equipment=std::max(0,budget.equipment-1);}
        else fallback(game,room,EntityKind::PressureRat,1,budget);
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::RepairBay && available(budget,room.role,2)) {
        if (result(budget,room.role,get_entity(game,populate_repair_bay(game,plan,room))!=nullptr)) budget.threat-=2;
        else fallback(game,room,EntityKind::ArcWelder,2,budget);
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::ScrapYard && available(budget,room.role,2)) {
        if (result(budget,room.role,get_entity(game,populate_scrap_yard(game,plan,room))!=nullptr)) {budget.threat-=2;budget.equipment=std::max(0,budget.equipment-2);}
        else fallback(game,room,EntityKind::MagnetCrane,2,budget);
    }
    // Reserve crew budget before incidental encounters consume it.
    for (const RoomPlan& room:plan.rooms)
        if (room.role==RoomRole::Workfront && available(budget,room.role,5)) {
            const int crew=populate_workfront(game,room);
            if (result(budget,room.role,crew>0)) budget.threat-=crew>=5 ? 8 : 5;
        }
    bool assembly=false;
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::AssemblyLine) {
        assembly=true; assembly_supplies(game,room);
        budget.equipment=std::max(0,budget.equipment-2);
        if (available(budget,room.role,2)) {
            if (result(budget,room.role,populate_rivet_post(game,plan,room))) budget.threat-=2;
            else fallback(game,room,EntityKind::RivetGunner,2,budget);
        }
    }
    for (const RoomPlan& room:plan.rooms)
        if (room.role==RoomRole::BlastingAlcove) {
            spawn_room_enemy(game,room,EntityKind::PowderMonkey,2,budget);
            if (!assembly && available(budget,room.role,2)) {
                if (result(budget,room.role,populate_rivet_post(game,plan,room))) budget.threat-=2;
                else fallback(game,room,EntityKind::RivetGunner,2,budget);
            }
            if (const auto cell=room_space(game,room)) place_ground_item(game,*cell,ItemKind::FuseScissors);
        }
}
