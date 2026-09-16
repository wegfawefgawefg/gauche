#include "industrial_population.hpp"
#include "forest_den.hpp"
#include "spider_cave.hpp"
#include "snake_tunnel.hpp"
#include "bear_stream.hpp"
#include "bear_clearings.hpp"
#include "brawlers.hpp"
#include "room_supplies.hpp"
#include "equipment_supply.hpp"
#include "../items/supply.hpp"
#include "route.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "ice_terrain.hpp"
#include "lens_watch.hpp"
#include "crystal_gallery.hpp"
#include "boiler_room.hpp"
#include "sluice.hpp"
#include "salvage.hpp"
#include "../surfaces/interaction.hpp"
#include "../entities/dispatch.hpp"
#include "../entities/seal_thief.hpp"
#include "../entities/boiler_porter.hpp"

#include <algorithm>
#include <optional>
#include <vector>

namespace {

void rooted_watch(Game& game, const RoomPlan& room, RoomSupplies& budget, bool guarded) {
    const Handle root_handle = spawn_room_enemy(game, room, EntityKind::RootTurret, 2, budget);
    const Entity* root = get_entity(game, root_handle);
    if (root == nullptr || !guarded || budget.threat < 2) return;
    for (Cell side : {Cell{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
        const Cell cell = root->cell + side;
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile) || entity_at(game, cell, false) >= 0 ||
            distance(cell, game.run.spawn) < 4) continue;
        Entity* guard = get_entity(game, spawn_entity(game, EntityKind::BrambleGuard, cell));
        if (guard != nullptr) { guard->entity_a = root_handle; budget.threat -= 2; }
        break;
    }
}

void room_encounter(Game& game, const FloorPlan& plan, const RoomPlan& room, RoomSupplies& budget) {
    if (room.shape==RoomShape::ThawCavern || reserved_habitat(room)) return;
    const int round = (game.run.floor - 1) % 4;
    if (room.role==RoomRole::Workfront || room.role==RoomRole::BlastingAlcove || room.role==RoomRole::AssemblyLine || room.role==RoomRole::RepairBay || room.role==RoomRole::ScrapYard || room.role==RoomRole::CoolingWorks || room.role==RoomRole::CableTrench || room.role==RoomRole::KilnCourt || room.role==RoomRole::PayOffice || room.role==RoomRole::LampAlcove || room.role==RoomRole::SlagBank || room.role==RoomRole::AshLoft || room.role==RoomRole::HoistShaft || room.role==RoomRole::CastingFloor || room.role==RoomRole::SettlingTanks || room.role==RoomRole::FreightSiding) return;
    if (ice_floor(game.run.floor) && (room.role == RoomRole::Reservoir ||
        room.role == RoomRole::IceQuarry || room.role == RoomRole::FishingHut)) {
        if (room.role == RoomRole::IceQuarry) spawn_room_enemy(game, room, EntityKind::IceMason, 2, budget);
        else if (room.role == RoomRole::FishingHut) spawn_room_enemy(game, room, EntityKind::FishingWidow, 2, budget);
        else if (room.role == RoomRole::Reservoir && round % 2 == 1) spawn_room_enemy(game, room, EntityKind::BellDiver, 2, budget);
        else spawn_room_enemy(game, room, EntityKind::RimeSkater, 2, budget);
        if (room.role == RoomRole::Reservoir) {
            spawn_room_enemy(game, room, EntityKind::GlassEel, 2, budget);
            spawn_room_enemy(game, room, EntityKind::SealThief, 1, budget);
        }
        else if (room.role == RoomRole::IceQuarry) spawn_room_enemy(game, room, EntityKind::SnowBurrower, 1, budget);
        else {
            spawn_room_enemy(game, room, EntityKind::SealThief, 1, budget);
            if (round >= 2) spawn_room_enemy(game, room, EntityKind::BellDiver, 2, budget);
        }
        return;
    }
    if (ice_floor(game.run.floor) && (room.role == RoomRole::Bathhouse || room.role == RoomRole::Shelter)) {
        if (room.role == RoomRole::Shelter) spawn_room_enemy(game, room, EntityKind::FrozenPilgrim, 2, budget);
        else if (round % 2 == 1) spawn_room_enemy(game, room, EntityKind::BoilerPorter, 3, budget);
        else spawn_room_enemy(game, room, EntityKind::SteamLeech, 2, budget);
        if (round >= 2) spawn_room_enemy(game, room, room.role == RoomRole::Shelter ?
            EntityKind::SteamLeech : EntityKind::FrozenPilgrim, 2, budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::Observatory) {
        bool warden = false;
        if (round >= 2 && budget.threat >= 4) {
            warden = get_entity(game, populate_lens_watch(game, room)) != nullptr;
            if (warden) budget.threat -= 4;
        }
        if (!warden) spawn_room_enemy(game, room, EntityKind::MirrorKnight, 3, budget);
        if (round >= 2) spawn_room_enemy(game, room, EntityKind::FrostBat, 2, budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::BoilerGallery) {
        place_maintenance_locker(game,plan,room);
        spawn_room_enemy(game,room,EntityKind::BoilerPorter,3,budget);
        if (round >= 2) spawn_room_enemy(game,room,EntityKind::SteamLeech,2,budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::ServicePassage) {
        spawn_room_enemy(game,room,EntityKind::IcicleSpider,2,budget);
        if (round >= 2) spawn_room_enemy(game,room,EntityKind::IcicleSpider,2,budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::CrystalGallery) {
        if (budget.threat >= 3 && get_entity(game,populate_crystal_gallery(game,plan,room))) budget.threat -= 3;
        if (round >= 2) spawn_room_enemy(game,room,EntityKind::FrostBat,2,budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::Chapel) {
        const Handle handle = spawn_room_enemy(game,room,EntityKind::CandleKeeper,2,budget);
        if (Entity* keeper = get_entity(game,handle)) keeper->point_a = room.center;
        if (round >= 2) spawn_room_enemy(game,room,EntityKind::SnowEffigy,2,budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::MemorialCourt) {
        spawn_room_enemy(game, room, EntityKind::SnowEffigy, 2, budget);
        if (round >= 2) spawn_room_enemy(game, room, EntityKind::EchoHound, 2, budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::CliffPath) {
        spawn_room_enemy(game, room, random_u32(game)%2==0 ? EntityKind::Yeti : EntityKind::AvalancheRam, 2, budget);
        if (round >= 2) spawn_room_enemy(game, room, EntityKind::SnowBurrower, 1, budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::WeatherStation) {
        spawn_room_enemy(game, room, EntityKind::WhiteoutDrummer, 1, budget);
        spawn_room_enemy(game, room, round >= 2 ? EntityKind::EchoHound : EntityKind::FrostBat, 2, budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::EchoTunnel) {
        spawn_room_enemy(game, room, EntityKind::EchoHound, 2, budget);
        if (round >= 2) spawn_room_enemy(game, room, EntityKind::FrostBat, 2, budget);
        return;
    }
    if (!forest_floor(game.run.floor)) {
        const auto roll=ice_floor(game.run.floor) ? 0U : random_u32(game)%9;
        const EntityKind hazard = ice_floor(game.run.floor) ? EntityKind::FrostBat :
            roll==0 ? EntityKind::PressureRat : roll==1 ? EntityKind::RivetGunner : roll==2 ? EntityKind::CableCrawler : roll==3 ? EntityKind::WalkingKiln : roll==4 ? EntityKind::FurnaceMoth : roll==5 ? EntityKind::SlagSnail : roll==6 ? EntityKind::AshSleeper : roll==7 ? EntityKind::EmergencyPump : EntityKind::Ember;
        const int cost=(hazard==EntityKind::PressureRat || hazard==EntityKind::FurnaceMoth) ? 1 : hazard==EntityKind::WalkingKiln ? 3 : 2;
        spawn_room_enemy(game, room, hazard, cost, budget);
        if (round >= 2) spawn_room_enemy(game, room, hazard, cost, budget);
        return;
    }
    switch (room.role) {
    case RoomRole::Thicket:
        if (round > 0 && random_u32(game) % 3 == 0) { spawn_room_enemy(game, room, EntityKind::BurrowWorm, 3, budget); break; }
        if (random_u32(game) % 3 == 0) rooted_watch(game, room, budget, round > 0);
        else spawn_room_enemy(game, room, random_u32(game) % 2 == 0 ? EntityKind::Wolf : EntityKind::Boar, 2, budget);
        if (round > 0) spawn_room_enemy(game, room, EntityKind::ThornSnail, 2, budget);
        break;
    case RoomRole::Brook:
        spawn_room_enemy(game, room, random_u32(game) % 2 == 0 ? EntityKind::SporeToad : EntityKind::LanternMoth, 1, budget);
        if (round > 0) spawn_room_enemy(game, room, random_u32(game) % 2 == 0 ? EntityKind::Bat : EntityKind::Mosquito, 1, budget);
        if (const auto cell = room_space(game, room)) spawn_entity(game, EntityKind::Bunny, *cell);
        break;
    case RoomRole::Ruins:
        if (random_u32(game) % 3 == 0) spawn_room_enemy(game, room, EntityKind::CarrionCrow, 1, budget);
        spawn_room_enemy(game, room, round >= 2 ? EntityKind::ZombieStack : EntityKind::Zombie,
              round >= 2 ? 3 : 1, budget);
        break;
    case RoomRole::Den:
        if (round > 0 && random_u32(game) % 3 == 0) { spawn_room_enemy(game, room, EntityKind::BurrowWorm, 3, budget); break; }
        if (random_u32(game) % 3 == 0) spawn_room_enemy(game, room, EntityKind::CarrionCrow, 1, budget);
        spawn_room_enemy(game, room, EntityKind::Wolf, 2, budget);
        if (round > 0) spawn_room_enemy(game, room, EntityKind::Den, 4, budget);
        break;
    case RoomRole::Cache:
        if (random_u32(game) % 2 == 0) spawn_room_enemy(game, room, EntityKind::CrateMimic, 2, budget);
        else spawn_room_enemy(game, room, EntityKind::ThornSnail, 2, budget);
        break;
    case RoomRole::Shrine:
        if (random_u32(game) % 2 == 0) { rooted_watch(game, room, budget, round > 0); break; }
        spawn_room_enemy(game, room, round > 0 ? EntityKind::Bear : EntityKind::Wolf,
              round > 0 ? 3 : 2, budget);
        break;
    case RoomRole::Orchard:
        if (round > 0 || random_u32(game) % 2 == 0) spawn_room_enemy(game, room, EntityKind::WaspNest, 3, budget);
        if (const auto cell = room_space(game, room)) spawn_chicken_family(game, *cell);
        break;
    case RoomRole::Workshop:
        if (random_u32(game) % 2 == 0) spawn_room_enemy(game, room, EntityKind::ForagerGoblin, 1, budget);
        if (random_u32(game) % 2 == 0) spawn_room_enemy(game, room, EntityKind::Woodpecker, 2, budget);
        if (round > 0 && random_u32(game) % 2 == 0) spawn_room_enemy(game, room, EntityKind::CrateMimic, 2, budget);
        if (const auto cell = room_space(game, room)) spawn_entity(game, EntityKind::Dog, *cell);
        break;
    case RoomRole::Clearing:
        if (random_u32(game) % 2 == 0) spawn_room_enemy(game, room, EntityKind::Owl, 2, budget);
        else spawn_room_enemy(game, room, EntityKind::Mosquito, 1, budget);
        break;
    default: break;
    }
}

void encounter(Game& game,const FloorPlan& plan,const RoomPlan& room,RoomSupplies& budget) {
    if (room.shape==RoomShape::ThawCavern || reserved_habitat(room)) return;
    if (!budget.report || room.role>=RoomRole::Workfront) {room_encounter(game,plan,room,budget);return;}
    const auto occupants=[&]() {return std::count_if(game.entities.begin(),game.entities.end(),
        [](const Entity& entity){return entity.kind!=EntityKind::None && entity.health>0;});};
    const auto before=occupants();
    int blocked=0;
    for (const auto& count:budget.report->enemies) blocked+=count.budget_blocked;
    room_encounter(game,plan,room,budget);
    auto& result=budget.report->scenes[static_cast<std::size_t>(room.role)];
    ++result.attempted;
    if (occupants()>before) ++result.placed;
    else {
        int after=0;
        for (const auto& count:budget.report->enemies) after+=count.budget_blocked;
        if (after>blocked || budget.threat<2) ++result.budget_blocked;
        else ++result.rejected;
    }
}

void supply(Game& game, const RoomPlan& room, ItemKind kind, int count, int& remaining, PopulationReport* report) {
    auto* tally=report ? &report->supplies[static_cast<std::size_t>(kind)] : nullptr;
    if (remaining<=0) {if (tally) ++tally->budget_blocked;return;}
    if (tally) ++tally->attempted;
    if (const auto cell=room_space(game,room)) {
        if (get_entity(game,place_ground_item(game,*cell,kind,count))) {
            --remaining;if (tally) ++tally->placed;return;
        }
    }
    if (tally) ++tally->rejected;
}

void stash(Game& game, const RoomPlan& room, RoomSupplies& budget) {
    if (budget.stashes <= 0) return;
    if (const auto cell = room_space(game, room)) {
        place_coins(game, *cell, 6 + static_cast<int>(random_u32(game) % 7));
        --budget.stashes;
    }
}

void room_loot(Game& game, const RoomPlan& room, RoomSupplies& budget) {
    const int round = (game.run.floor - 1) % 4;
    // RESERVOIR: Shared supplies bridge the regional catalog as its items arrive.
    if (ice_floor(game.run.floor)) {
        if (room.role == RoomRole::Cache || room.role == RoomRole::Observatory ||
            room.role == RoomRole::Secret || room.role == RoomRole::Shrine) stash(game, room, budget);
        if (room.role == RoomRole::Shrine && random_u32(game)%3==0)
            supply(game,room,ItemKind::BorrowedSummer,1,budget.equipment,budget.report);
        if (room.role == RoomRole::IceQuarry && random_u32(game)%3==0)
            supply(game,room,ItemKind::ThawCharge,2,budget.equipment,budget.report);
        if (room.role == RoomRole::Reservoir || room.role == RoomRole::IceQuarry) {
            if (room.role == RoomRole::IceQuarry) {
                const ItemKind tool=round%3==0 ? ItemKind::IceAxe : round%3==1 ? ItemKind::Chisel : ItemKind::IceBrick;
                supply(game,room,tool,tool==ItemKind::IceBrick ? 2 : 1,budget.equipment,budget.report);
            }
            else {
                supply(game, room, ItemKind::GritPouch, 1, budget.equipment,budget.report);
                supply(game, room, ItemKind::SaltedKelp, 2, budget.healing,budget.report);
            }
        } else if (room.role == RoomRole::FishingHut) {
            supply(game, room, ItemKind::SmokedFish, 2, budget.healing,budget.report);
            constexpr ItemKind fishing_tools[]{ItemKind::FishingLine,ItemKind::TuskPike,ItemKind::HarpoonGun,ItemKind::FoldedBridge};
            supply(game,room,fishing_tools[round % 4],1,budget.equipment,budget.report);
        } else if (room.role == RoomRole::Shelter || room.role == RoomRole::Bathhouse) {
            if (room.role == RoomRole::Bathhouse) {
                supply(game,room,ItemKind::CoalLump,4,budget.ammunition,budget.report);
                supply(game,room,ItemKind::SteamKettle,1,budget.equipment,budget.report);
            }
            supply(game, room, room.role == RoomRole::Shelter ? ItemKind::HotBroth : ItemKind::IcePoultice, 2, budget.healing,budget.report);
            supply(game, room, room.role == RoomRole::Shelter ? (round%3==0 ? ItemKind::SnowShelter : round%3==1 ? ItemKind::SnowScoop : ItemKind::WoolWrap) : ItemKind::HeatCapsule,
                room.role == RoomRole::Shelter && round%3!=2 ? 1 : 2, budget.equipment,budget.report);
        } else if (room.role == RoomRole::BoilerGallery) {
            if (random_u32(game)%4==0) supply(game,room,ItemKind::HeatSiphon,1,budget.equipment,budget.report);
            supply(game,room,round%2 == 0 ? ItemKind::PressureValve : ItemKind::Sealant,1,budget.equipment,budget.report);
        } else if (room.role == RoomRole::ServicePassage) {
            supply(game,room,round%2==0 ? ItemKind::CopperWire : ItemKind::GroundingSpike,1,budget.equipment,budget.report);
        } else if (room.role == RoomRole::CrystalGallery) {
            supply(game,room,round%2==0 ? ItemKind::TuningFork : ItemKind::IceBrick,round%2==0 ? 1 : 2,budget.equipment,budget.report);
        } else if (room.role == RoomRole::Chapel) {
            supply(game,room,ItemKind::BrineFlask,2,budget.equipment,budget.report);
        } else if (room.role == RoomRole::MemorialCourt) {
            supply(game, room, round%2==0 ? ItemKind::EffigyMask : ItemKind::HeatCapsule, round%2==0 ? 1 : 2, budget.equipment,budget.report);
        } else if (room.role == RoomRole::CliffPath) {
            supply(game, room, round%4==0 ? ItemKind::IceAnchor : round%4==1 ? ItemKind::Crampons : round%4==2 ? ItemKind::Sled : ItemKind::WoolWrap, 1, budget.equipment,budget.report);
        } else if (room.role == RoomRole::WeatherStation) {
            supply(game, room, round%3==0 ? ItemKind::StormLantern : round%3==1 ? ItemKind::SignalFlare : ItemKind::SnowGlobe, 1, budget.equipment,budget.report);
        } else if (room.role == RoomRole::EchoTunnel) {
            constexpr ItemKind quiet_tools[]{ItemKind::IceNeedle, ItemKind::MufflingFelt, ItemKind::AlarmClock, ItemKind::EchoPebble};
            supply(game, room, quiet_tools[round % 4], round % 4 == 0 ? 3 : 1, budget.equipment,budget.report);
        } else if (room.role == RoomRole::Secret || room.role == RoomRole::Cache) {
            const ItemKind kind=roll_item_supply(game,room.role==RoomRole::Secret ? LootSource::Secret : LootSource::Cache);
            supply(game,room,kind,supply_count(kind),budget.equipment,budget.report);
            supply(game, room, ItemKind::Ammo, 2, budget.ammunition,budget.report);
        } else if (room.role == RoomRole::Observatory || room.role == RoomRole::Shrine) {
            supply(game, room, room.role == RoomRole::Observatory ? (round % 2 == 0 ? ItemKind::LensCarbine : ItemKind::PrismBomb) : ItemKind::ColdFlask,
                room.role == RoomRole::Observatory ? 1 : 2, budget.equipment,budget.report);
            supply(game, room, ItemKind::Ammo, 2, budget.ammunition,budget.report);
        }
        return;
    }
    if (industrial_floor(game.run.floor)) {
        if (room.role==RoomRole::Cache || room.role==RoomRole::Secret ||
            room.role==RoomRole::Workshop || room.role==RoomRole::Shrine) {
            stash(game,room,budget);
            const LootSource source=room.role==RoomRole::Secret ? LootSource::Secret :
                room.role==RoomRole::Workshop ? LootSource::Workshop : LootSource::Cache;
            const ItemKind kind=roll_item_supply(game,source);
            supply(game,room,kind,supply_count(kind),budget.equipment,budget.report);
            supply(game,room,ItemKind::Ammo,1,budget.ammunition,budget.report);
            if (random_u32(game)%2==0) supply(game,room,ItemKind::LunchTin,1,budget.healing,budget.report);
            else supply(game,room,ItemKind::Bandage,2,budget.healing,budget.report);
        }
        return;
    }
    if (room.role == RoomRole::Cache || room.role == RoomRole::Shrine ||
        room.role == RoomRole::Workshop || room.role == RoomRole::Secret) stash(game, room, budget);
    switch (room.role) {
    case RoomRole::Secret:
        if (const auto cell = room_space(game, room))
        {
            const ItemKind kind=roll_item_supply(game,LootSource::Secret);
            place_ground_item(game,*cell,kind,supply_count(kind));
        }
        supply(game, room, ItemKind::Ammo, 1, budget.ammunition,budget.report);
        break;
    case RoomRole::Workshop: {
        const ItemKind kind=roll_item_supply(game,LootSource::Workshop,false);
        supply(game,room,kind,supply_count(kind),budget.equipment,budget.report);
        supply(game, room, ItemKind::Ammo, 1, budget.ammunition,budget.report);
        break;
    }
    case RoomRole::Cache: {
        const ItemKind kind=roll_item_supply(game,LootSource::Cache);
        supply(game,room,kind,supply_count(kind),budget.equipment,budget.report);
        {
            constexpr ItemKind healing[]{ItemKind::Bandage, ItemKind::HerbBag, ItemKind::FungalBread};
            const ItemKind remedy = healing[random_u32(game) % std::size(healing)];
            supply(game, room, remedy, remedy == ItemKind::Bandage ? 2 : 1, budget.healing,budget.report);
        }
        break;
    }
    case RoomRole::Brook:
        supply(game, room, random_u32(game) % 2 == 0 ? ItemKind::WaterFlask : ItemKind::MushroomSpores,
               2, budget.equipment,budget.report);
        break;
    case RoomRole::Thicket: {
        constexpr ItemKind forest_tools[]{ItemKind::Torch, ItemKind::Lighter, ItemKind::OilFlask, ItemKind::SapJar,
            ItemKind::SeedBag, ItemKind::LanternSeed, ItemKind::BitterRoot, ItemKind::Chili, ItemKind::ThornCaltrops};
        supply(game, room, forest_tools[random_u32(game) % std::size(forest_tools)], 1, budget.equipment,budget.report);
        break;
    }
    case RoomRole::Shrine:
        supply(game, room, ItemKind::Medkit, 1, budget.healing,budget.report);
        break;
    case RoomRole::Orchard: case RoomRole::Clearing:
        supply(game, room, random_u32(game) % 2 == 0 ? ItemKind::BirdSeed : ItemKind::Rake, 1, budget.equipment,budget.report);
        {
            constexpr ItemKind healing[]{ItemKind::Bandage, ItemKind::HerbBag, ItemKind::FungalBread};
            const ItemKind remedy = healing[random_u32(game) % std::size(healing)];
            supply(game, room, remedy, remedy == ItemKind::Bandage ? 2 : 1, budget.healing,budget.report);
        }
        break;
    default:
        if (random_u32(game) % 3 == 0) supply(game, room, ItemKind::Ammo, 1, budget.ammunition,budget.report);
        break;
    }
}

void room_light(Game& game, const RoomPlan& room) {
    // WARMTH: An actual campfire marks shelter; cold rooms have no invented skylight.
    if (ice_floor(game.run.floor)) {
        if (room.role == RoomRole::Entrance || room.role == RoomRole::Shelter ||
            room.role == RoomRole::Bathhouse || room.role == RoomRole::FishingHut) {
            if (const auto cell = room_space(game, room)) spawn_entity(game, EntityKind::Campfire, *cell);
        }
        return;
    }
    if (!forest_floor(game.run.floor)) return;
    if (room.role == RoomRole::Clearing || room.role == RoomRole::Orchard || room.role == RoomRole::Brook) {
        if (game.run.roof_light_count >= static_cast<int>(game.run.roof_lights.size())) return;
        StageLight& gap = game.run.roof_lights[static_cast<std::size_t>(game.run.roof_light_count++)];
        gap = {room.center, {std::min(room.half_width, room.half_height), 1350, {240, 224, 176}}};
    } else if (room.role == RoomRole::Entrance || room.role == RoomRole::Workshop) {
        if (const auto cell = room_space(game, room)) spawn_entity(game, EntityKind::Campfire, *cell);
    }
}

} // namespace

void populate_rooms(Game& game, const FloorPlan& plan, PopulationReport* report) {
    const int round = (game.run.floor - 1) % 4;
    RoomSupplies budget{9 + round * 5, 2 + round / 2, 2 + round, 3, 3 + round / 2};
    budget.report=report;
    if (report) {
        report->rooms=plan.rooms;report->forest_dens=plan.forest_dens;report->spider_caves=plan.spider_caves;report->snake_tunnels=plan.snake_tunnels;
        report->industry_profile=plan.industry_profile;report->industrial_links=plan.industrial_links;
        report->thaw_channels=plan.thaw_channels;
        report->shelf_links=plan.shelf_links;report->shelf_rewards=plan.shelf_rewards;
        for (const RoomPlan& room:plan.rooms) {
            ++report->scenes[static_cast<std::size_t>(room.role)].planned;
            report->shelf_rooms+=room.shape==RoomShape::IceShelf;
        }
    }
    game.run.roof_lights = {};
    game.run.roof_light_count = 0;
    // LANDMARKS: Reserve objectives before any scatter or encounter placement.
    const Cell objective = plan.rooms[static_cast<std::size_t>(plan.objective_room)].center;
    spawn_entity(game, game.run.objective == ObjectiveKind::Key ? EntityKind::Key : EntityKind::Switch, objective);
    spawn_entity(game, EntityKind::Door, plan.door);
    spawn_entity(game, EntityKind::Exit, game.run.exit);
    ItemKind starter=ItemKind::Stick;
    if (forest_floor(game.run.floor))
        place_ground_item(game,game.run.spawn+Cell{0,2},ItemKind::Stick);
    else {
        // Reserve one ordinary equipment slot for a native combat tool before
        // situational role supplies can consume the floor's entire budget.
        starter=roll_item_supply(game,LootSource::Weapon,false);
        if (starter!=ItemKind::None && get_entity(game,place_ground_item(game,
            game.run.spawn+Cell{0,2},starter,supply_count(starter)))) --budget.equipment;
    }
    RoomSupplies fighters{};fighters.report=report;
    if (ice_floor(game.run.floor) || industrial_floor(game.run.floor)) {
        fighters.threat=6+round*2;
        // Keep the previous specialist allowance; extra ordinary combat must
        // not replace the crews and scenes that make these biomes distinct.
        budget.threat-=3+round;
    }
    // Installations contain both inhabitants and their working tools. Incidental
    // encounters and the starting weapon cannot silently evict that whole scene.
    if (industrial_floor(game.run.floor)) {
        RoomSupplies installations{16+round*3,0,0,6+round,0,report};
        populate_industrial_rooms(game,plan,installations);
    }
    if (!forest_floor(game.run.floor)) {
        // Fit terrain-changing side scenes before actors claim their footprints.
        bool salvage=false;
        std::vector<std::size_t> encounters;
        for (std::size_t i=0;i<plan.rooms.size();++i) {
            const RoomPlan& room=plan.rooms[i];
            room_light(game,room);
            if (room.role==RoomRole::Entrance || room.role==RoomRole::Exit) continue;
            place_sluice_chamber(game,plan,room);
            if (!salvage) salvage=place_salvage_pocket(game,plan,room);
            encounters.push_back(i);
        }
        for (std::size_t i=encounters.size();i>1;--i)
            std::swap(encounters[i-1],encounters[random_u32(game)%i]);
        for (auto index:encounters) encounter(game,plan,plan.rooms[index],budget);
    }
    populate_forest_den(game,plan);
    populate_spider_cave(game,plan);
    populate_snake_tunnel(game,plan);
    populate_bear_streams(game,plan,report);
    const auto bear_rooms=populate_bear_clearings(game,plan,report);
    if (forest_floor(game.run.floor)) {
        std::vector<std::size_t> encounters;
        for (std::size_t i=0;i<plan.rooms.size();++i) {
            const auto& room=plan.rooms[i];
            if (room.role!=RoomRole::Entrance && room.role!=RoomRole::Exit &&
                room.role!=RoomRole::Secret && !reserved_habitat(room) &&
                std::find(bear_rooms.begin(),bear_rooms.end(),i)==bear_rooms.end()) encounters.push_back(i);
        }
        // A growing floor needs a growing population. Shuffle allocation so deep
        // branches don't become empty after early rooms spend the shared budget.
        budget.threat=std::max(budget.threat,static_cast<int>(encounters.size())*2+round*3);
        for (std::size_t i=encounters.size();i>1;--i)
            std::swap(encounters[i-1],encounters[random_u32(game)%i]);
        for (const auto i:encounters) encounter(game,plan,plan.rooms[i],budget);
    }
    place_field_equipment(game,plan,budget,starter);
    for (const RoomPlan& room:plan.rooms) {
        if (forest_floor(game.run.floor)) room_light(game,room);
        if (room.role==RoomRole::Entrance || room.role==RoomRole::Exit) continue;
        if (reserved_habitat(room)) continue;
        place_crystal_vein(game,plan,room);
        room_loot(game,room,budget);
    }
    place_brawlers(game,plan,fighters);
    // SUPPLIES: A sparse role roll must not accidentally remove all healing or new equipment.
    const RoomPlan& shrine = plan.rooms[static_cast<std::size_t>(plan.objective_room)];
    while (budget.healing > 0) {
        const int before = budget.healing;
        supply(game, shrine, ItemKind::Bandage, 2, budget.healing,budget.report);
        if (before == budget.healing) break;
    }
    if (budget.equipment > 0) {
        const ItemKind weapon=roll_item_supply(game,LootSource::Weapon,false);
        if (weapon!=ItemKind::None) supply(game,shrine,weapon,supply_count(weapon),budget.equipment,budget.report);
    }
    if (budget.ammunition > 0) supply(game, shrine, ItemKind::Ammo, 1, budget.ammunition,budget.report);
    while (budget.stashes > 0) {
        const int before = budget.stashes;
        stash(game, shrine, budget);
        if (before == budget.stashes) break;
    }
}
