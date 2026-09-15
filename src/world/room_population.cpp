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
#include "../items/supply.hpp"
#include "rivet_post.hpp"
#include "assembly.hpp"
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

struct Supplies { int threat, healing, ammunition, equipment, stashes; };

std::optional<Cell> room_space(Game& game, const RoomPlan& room, EntityKind kind = EntityKind::None) {
    const int width = room.half_width * 2 + 1;
    std::vector<bool> seen(static_cast<std::size_t>(width * (room.half_height * 2 + 1)), false);
    std::vector<Cell> queue{room.center}, choices;
    constexpr Cell sides[]{{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (std::size_t next = 0; next < queue.size(); ++next) {
        const Cell cell = queue[next];
        const int x = cell.x - room.center.x + room.half_width;
        const int y = cell.y - room.center.y + room.half_height;
        if (x < 0 || x >= width || y < 0 || y > room.half_height * 2) continue;
        const auto index = static_cast<std::size_t>(y * width + x);
        if (seen[index]) continue;
        seen[index] = true;
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile) || tile->kind == TileKind::Lava) continue;
        if (distance(cell, game.run.spawn) >= 4 && entity_at(game, cell, false) < 0 &&
            tile->kind != TileKind::Spring && (kind != EntityKind::RimeSkater || tile->kind == TileKind::Ice) &&
            (kind != EntityKind::BellDiver || tile->kind == TileKind::IceHole) &&
            (kind != EntityKind::GlassEel || surface_wet(*tile)) &&
            (kind != EntityKind::SnowBurrower || tile->kind == TileKind::Snow) &&
            (kind != EntityKind::SealThief || seal_bank(game,cell)))
            choices.push_back(cell);
        for (Cell side : sides) queue.push_back(cell + side);
    }
    // ISLANDS: Required supplies never roll onto a bank isolated by water or lava.
    if (choices.empty()) return std::nullopt;
    return choices[random_u32(game) % choices.size()];
}

Handle enemy(Game& game, const RoomPlan& room, EntityKind kind, int cost, Supplies& budget) {
    if (cost > budget.threat) return {};
    if (const auto cell = room_space(game, room, kind)) {
        const Handle spawned = kind == EntityKind::BurrowWorm ?
            spawn_burrow_worm(game, *cell) : kind == EntityKind::BoilerPorter ?
            spawn_boiler_porter(game,*cell) : spawn_entity(game, kind, *cell);
        if (get_entity(game, spawned) != nullptr) budget.threat -= cost;
        return spawned;
    }
    return {};
}

void rooted_watch(Game& game, const RoomPlan& room, Supplies& budget, bool guarded) {
    const Handle root_handle = enemy(game, room, EntityKind::RootTurret, 2, budget);
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

void encounter(Game& game, const FloorPlan& plan, const RoomPlan& room, Supplies& budget) {
    const int round = (game.run.floor - 1) % 4;
    if (room.role==RoomRole::Workfront || room.role==RoomRole::BlastingAlcove || room.role==RoomRole::AssemblyLine || room.role==RoomRole::RepairBay || room.role==RoomRole::ScrapYard || room.role==RoomRole::CoolingWorks || room.role==RoomRole::CableTrench || room.role==RoomRole::KilnCourt || room.role==RoomRole::PayOffice || room.role==RoomRole::LampAlcove || room.role==RoomRole::SlagBank || room.role==RoomRole::AshLoft) return;
    if (ice_floor(game.run.floor) && (room.role == RoomRole::Reservoir ||
        room.role == RoomRole::IceQuarry || room.role == RoomRole::FishingHut)) {
        if (room.role == RoomRole::IceQuarry) enemy(game, room, EntityKind::IceMason, 2, budget);
        else if (room.role == RoomRole::FishingHut) enemy(game, room, EntityKind::FishingWidow, 2, budget);
        else if (room.role == RoomRole::Reservoir && round % 2 == 1) enemy(game, room, EntityKind::BellDiver, 2, budget);
        else enemy(game, room, EntityKind::RimeSkater, 2, budget);
        if (room.role == RoomRole::Reservoir) {
            enemy(game, room, EntityKind::GlassEel, 2, budget);
            enemy(game, room, EntityKind::SealThief, 1, budget);
        }
        else if (room.role == RoomRole::IceQuarry) enemy(game, room, EntityKind::SnowBurrower, 1, budget);
        else {
            enemy(game, room, EntityKind::SealThief, 1, budget);
            if (round >= 2) enemy(game, room, EntityKind::BellDiver, 2, budget);
        }
        return;
    }
    if (ice_floor(game.run.floor) && (room.role == RoomRole::Bathhouse || room.role == RoomRole::Shelter)) {
        if (room.role == RoomRole::Shelter) enemy(game, room, EntityKind::FrozenPilgrim, 2, budget);
        else if (round % 2 == 1) enemy(game, room, EntityKind::BoilerPorter, 3, budget);
        else enemy(game, room, EntityKind::SteamLeech, 2, budget);
        if (round >= 2) enemy(game, room, room.role == RoomRole::Shelter ?
            EntityKind::SteamLeech : EntityKind::FrozenPilgrim, 2, budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::Observatory) {
        bool warden = false;
        if (round >= 2 && budget.threat >= 4) {
            warden = get_entity(game, populate_lens_watch(game, room)) != nullptr;
            if (warden) budget.threat -= 4;
        }
        if (!warden) enemy(game, room, EntityKind::MirrorKnight, 3, budget);
        if (round >= 2) enemy(game, room, EntityKind::FrostBat, 2, budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::BoilerGallery) {
        place_maintenance_locker(game,plan,room);
        enemy(game,room,EntityKind::BoilerPorter,3,budget);
        if (round >= 2) enemy(game,room,EntityKind::SteamLeech,2,budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::ServicePassage) {
        enemy(game,room,EntityKind::IcicleSpider,2,budget);
        if (round >= 2) enemy(game,room,EntityKind::IcicleSpider,2,budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::CrystalGallery) {
        if (budget.threat >= 3 && get_entity(game,populate_crystal_gallery(game,plan,room))) budget.threat -= 3;
        if (round >= 2) enemy(game,room,EntityKind::FrostBat,2,budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::Chapel) {
        const Handle handle = enemy(game,room,EntityKind::CandleKeeper,2,budget);
        if (Entity* keeper = get_entity(game,handle)) keeper->point_a = room.center;
        if (round >= 2) enemy(game,room,EntityKind::SnowEffigy,2,budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::MemorialCourt) {
        enemy(game, room, EntityKind::SnowEffigy, 2, budget);
        if (round >= 2) enemy(game, room, EntityKind::EchoHound, 2, budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::CliffPath) {
        enemy(game, room, random_u32(game)%2==0 ? EntityKind::Yeti : EntityKind::AvalancheRam, 2, budget);
        if (round >= 2) enemy(game, room, EntityKind::SnowBurrower, 1, budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::WeatherStation) {
        enemy(game, room, EntityKind::WhiteoutDrummer, 1, budget);
        enemy(game, room, round >= 2 ? EntityKind::EchoHound : EntityKind::FrostBat, 2, budget);
        return;
    }
    if (ice_floor(game.run.floor) && room.role == RoomRole::EchoTunnel) {
        enemy(game, room, EntityKind::EchoHound, 2, budget);
        if (round >= 2) enemy(game, room, EntityKind::FrostBat, 2, budget);
        return;
    }
    if (!forest_floor(game.run.floor)) {
        const auto roll=ice_floor(game.run.floor) ? 0U : random_u32(game)%8;
        const EntityKind hazard = ice_floor(game.run.floor) ? EntityKind::FrostBat :
            roll==0 ? EntityKind::PressureRat : roll==1 ? EntityKind::RivetGunner : roll==2 ? EntityKind::CableCrawler : roll==3 ? EntityKind::WalkingKiln : roll==4 ? EntityKind::FurnaceMoth : roll==5 ? EntityKind::SlagSnail : roll==6 ? EntityKind::AshSleeper : EntityKind::Ember;
        const int cost=(hazard==EntityKind::PressureRat || hazard==EntityKind::FurnaceMoth) ? 1 : hazard==EntityKind::WalkingKiln ? 3 : 2;
        enemy(game, room, hazard, cost, budget);
        if (round >= 2) enemy(game, room, hazard, cost, budget);
        return;
    }
    switch (room.role) {
    case RoomRole::Thicket:
        if (round > 0 && random_u32(game) % 3 == 0) { enemy(game, room, EntityKind::BurrowWorm, 3, budget); break; }
        if (random_u32(game) % 3 == 0) rooted_watch(game, room, budget, round > 0);
        else enemy(game, room, random_u32(game) % 2 == 0 ? EntityKind::Wolf : EntityKind::Boar, 2, budget);
        if (round > 0) enemy(game, room, EntityKind::ThornSnail, 2, budget);
        break;
    case RoomRole::Brook:
        enemy(game, room, random_u32(game) % 2 == 0 ? EntityKind::SporeToad : EntityKind::LanternMoth, 1, budget);
        if (round > 0) enemy(game, room, random_u32(game) % 2 == 0 ? EntityKind::Bat : EntityKind::Mosquito, 1, budget);
        if (const auto cell = room_space(game, room)) spawn_entity(game, EntityKind::Bunny, *cell);
        break;
    case RoomRole::Ruins:
        if (random_u32(game) % 3 == 0) enemy(game, room, EntityKind::CarrionCrow, 1, budget);
        enemy(game, room, round >= 2 ? EntityKind::ZombieStack : EntityKind::Zombie,
              round >= 2 ? 3 : 1, budget);
        break;
    case RoomRole::Den:
        if (round > 0 && random_u32(game) % 3 == 0) { enemy(game, room, EntityKind::BurrowWorm, 3, budget); break; }
        if (random_u32(game) % 3 == 0) enemy(game, room, EntityKind::CarrionCrow, 1, budget);
        enemy(game, room, EntityKind::Wolf, 2, budget);
        if (round > 0) enemy(game, room, EntityKind::Den, 4, budget);
        break;
    case RoomRole::Cache:
        if (random_u32(game) % 2 == 0) enemy(game, room, EntityKind::CrateMimic, 2, budget);
        else enemy(game, room, EntityKind::ThornSnail, 2, budget);
        break;
    case RoomRole::Shrine:
        if (random_u32(game) % 2 == 0) { rooted_watch(game, room, budget, round > 0); break; }
        enemy(game, room, round > 0 ? EntityKind::Bear : EntityKind::Wolf,
              round > 0 ? 3 : 2, budget);
        break;
    case RoomRole::Orchard:
        if (round > 0 || random_u32(game) % 2 == 0) enemy(game, room, EntityKind::WaspNest, 3, budget);
        if (const auto cell = room_space(game, room)) spawn_chicken_family(game, *cell);
        break;
    case RoomRole::Workshop:
        if (random_u32(game) % 2 == 0) enemy(game, room, EntityKind::ForagerGoblin, 1, budget);
        if (random_u32(game) % 2 == 0) enemy(game, room, EntityKind::Woodpecker, 2, budget);
        if (round > 0 && random_u32(game) % 2 == 0) enemy(game, room, EntityKind::CrateMimic, 2, budget);
        if (const auto cell = room_space(game, room)) spawn_entity(game, EntityKind::Dog, *cell);
        break;
    case RoomRole::Clearing:
        if (random_u32(game) % 2 == 0) enemy(game, room, EntityKind::Owl, 2, budget);
        else enemy(game, room, EntityKind::Mosquito, 1, budget);
        break;
    default: break;
    }
}

void supply(Game& game, const RoomPlan& room, ItemKind kind, int count, int& remaining) {
    if (remaining <= 0) return;
    if (const auto cell = room_space(game, room)) {
        place_ground_item(game, *cell, kind, count);
        --remaining;
    }
}

void stash(Game& game, const RoomPlan& room, Supplies& budget) {
    if (budget.stashes <= 0) return;
    if (const auto cell = room_space(game, room)) {
        place_coins(game, *cell, 6 + static_cast<int>(random_u32(game) % 7));
        --budget.stashes;
    }
}

void room_loot(Game& game, const RoomPlan& room, Supplies& budget) {
    const int round = (game.run.floor - 1) % 4;
    // RESERVOIR: Shared supplies bridge the regional catalog as its items arrive.
    if (ice_floor(game.run.floor)) {
        if (room.role == RoomRole::Cache || room.role == RoomRole::Observatory ||
            room.role == RoomRole::Secret || room.role == RoomRole::Shrine) stash(game, room, budget);
        if (room.role == RoomRole::Shrine && random_u32(game)%3==0)
            supply(game,room,ItemKind::BorrowedSummer,1,budget.equipment);
        if (room.role == RoomRole::IceQuarry && random_u32(game)%3==0)
            supply(game,room,ItemKind::ThawCharge,2,budget.equipment);
        if (room.role == RoomRole::Reservoir || room.role == RoomRole::IceQuarry) {
            if (room.role == RoomRole::IceQuarry) supply(game, room, round % 2 == 0 ? ItemKind::Chisel : ItemKind::IceBrick,
                round % 2 == 0 ? 1 : 2, budget.equipment);
            else {
                supply(game, room, ItemKind::GritPouch, 1, budget.equipment);
                supply(game, room, ItemKind::SaltedKelp, 2, budget.healing);
            }
        } else if (room.role == RoomRole::FishingHut) {
            supply(game, room, ItemKind::SmokedFish, 2, budget.healing);
            constexpr ItemKind fishing_tools[]{ItemKind::FishingLine,ItemKind::AirBladder,ItemKind::HarpoonGun,ItemKind::FoldedBridge};
            supply(game,room,fishing_tools[round % 4],1,budget.equipment);
        } else if (room.role == RoomRole::Shelter || room.role == RoomRole::Bathhouse) {
            if (room.role == RoomRole::Bathhouse) {
                supply(game,room,ItemKind::CoalLump,4,budget.ammunition);
                supply(game,room,ItemKind::SteamKettle,1,budget.equipment);
            }
            supply(game, room, room.role == RoomRole::Shelter ? ItemKind::HotBroth : ItemKind::IcePoultice, 2, budget.healing);
            supply(game, room, room.role == RoomRole::Shelter ? (round%3==0 ? ItemKind::SnowShelter : round%3==1 ? ItemKind::SnowScoop : ItemKind::WoolWrap) : ItemKind::HeatCapsule,
                room.role == RoomRole::Shelter && round%3!=2 ? 1 : 2, budget.equipment);
        } else if (room.role == RoomRole::BoilerGallery) {
            if (random_u32(game)%4==0) supply(game,room,ItemKind::HeatSiphon,1,budget.equipment);
            supply(game,room,round%2 == 0 ? ItemKind::PressureValve : ItemKind::Sealant,1,budget.equipment);
        } else if (room.role == RoomRole::ServicePassage) {
            supply(game,room,round%2==0 ? ItemKind::CopperWire : ItemKind::GroundingSpike,1,budget.equipment);
        } else if (room.role == RoomRole::CrystalGallery) {
            supply(game,room,round%2==0 ? ItemKind::TuningFork : ItemKind::IceBrick,round%2==0 ? 1 : 2,budget.equipment);
        } else if (room.role == RoomRole::Chapel) {
            supply(game,room,ItemKind::BrineFlask,2,budget.equipment);
        } else if (room.role == RoomRole::MemorialCourt) {
            supply(game, room, round%2==0 ? ItemKind::EffigyMask : ItemKind::HeatCapsule, round%2==0 ? 1 : 2, budget.equipment);
        } else if (room.role == RoomRole::CliffPath) {
            supply(game, room, round%4==0 ? ItemKind::IceAnchor : round%4==1 ? ItemKind::Crampons : round%4==2 ? ItemKind::Sled : ItemKind::WoolWrap, 1, budget.equipment);
        } else if (room.role == RoomRole::WeatherStation) {
            supply(game, room, round%3==0 ? ItemKind::StormLantern : round%3==1 ? ItemKind::SignalFlare : ItemKind::SnowGlobe, 1, budget.equipment);
        } else if (room.role == RoomRole::EchoTunnel) {
            constexpr ItemKind quiet_tools[]{ItemKind::IceNeedle, ItemKind::MufflingFelt, ItemKind::AlarmClock, ItemKind::EchoPebble};
            supply(game, room, quiet_tools[round % 4], round % 4 == 0 ? 3 : 1, budget.equipment);
        } else if (room.role == RoomRole::Secret || room.role == RoomRole::Cache) {
            const ItemKind kind=roll_item_supply(game,room.role==RoomRole::Secret ? LootSource::Secret : LootSource::Cache);
            supply(game,room,kind,supply_count(kind),budget.equipment);
            supply(game, room, ItemKind::Ammo, 2, budget.ammunition);
        } else if (room.role == RoomRole::Observatory || room.role == RoomRole::Shrine) {
            supply(game, room, room.role == RoomRole::Observatory ? (round % 2 == 0 ? ItemKind::LensCarbine : ItemKind::PrismBomb) : ItemKind::ColdFlask,
                room.role == RoomRole::Observatory ? 1 : 2, budget.equipment);
            supply(game, room, ItemKind::Ammo, 2, budget.ammunition);
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
            supply(game,room,kind,supply_count(kind),budget.equipment);
            supply(game,room,ItemKind::Ammo,1,budget.ammunition);
            supply(game,room,ItemKind::Bandage,2,budget.healing);
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
        supply(game, room, ItemKind::Ammo, 1, budget.ammunition);
        break;
    case RoomRole::Workshop: {
        const ItemKind kind=roll_item_supply(game,LootSource::Workshop,false);
        supply(game,room,kind,supply_count(kind),budget.equipment);
        supply(game, room, ItemKind::Ammo, 1, budget.ammunition);
        break;
    }
    case RoomRole::Cache: {
        const ItemKind kind=roll_item_supply(game,LootSource::Cache);
        supply(game,room,kind,supply_count(kind),budget.equipment);
        {
            constexpr ItemKind healing[]{ItemKind::Bandage, ItemKind::HerbBag, ItemKind::FungalBread};
            const ItemKind remedy = healing[random_u32(game) % std::size(healing)];
            supply(game, room, remedy, remedy == ItemKind::Bandage ? 2 : 1, budget.healing);
        }
        break;
    }
    case RoomRole::Brook:
        supply(game, room, random_u32(game) % 2 == 0 ? ItemKind::WaterFlask : ItemKind::MushroomSpores,
               2, budget.equipment);
        break;
    case RoomRole::Thicket: {
        constexpr ItemKind forest_tools[]{ItemKind::Torch, ItemKind::Lighter, ItemKind::OilFlask, ItemKind::SapJar,
            ItemKind::SeedBag, ItemKind::LanternSeed, ItemKind::BitterRoot, ItemKind::Chili, ItemKind::ThornCaltrops};
        supply(game, room, forest_tools[random_u32(game) % std::size(forest_tools)], 1, budget.equipment);
        break;
    }
    case RoomRole::Shrine:
        supply(game, room, ItemKind::Medkit, 1, budget.healing);
        break;
    case RoomRole::Orchard: case RoomRole::Clearing:
        supply(game, room, random_u32(game) % 2 == 0 ? ItemKind::BirdSeed : ItemKind::Rake, 1, budget.equipment);
        {
            constexpr ItemKind healing[]{ItemKind::Bandage, ItemKind::HerbBag, ItemKind::FungalBread};
            const ItemKind remedy = healing[random_u32(game) % std::size(healing)];
            supply(game, room, remedy, remedy == ItemKind::Bandage ? 2 : 1, budget.healing);
        }
        break;
    default:
        if (random_u32(game) % 3 == 0) supply(game, room, ItemKind::Ammo, 1, budget.ammunition);
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

void populate_rooms(Game& game, const FloorPlan& plan) {
    const int round = (game.run.floor - 1) % 4;
    Supplies budget{9 + round * 5, 2 + round / 2, 2 + round, 3, 3 + round / 2};
    game.run.roof_lights = {};
    game.run.roof_light_count = 0;
    // LANDMARKS: Reserve objectives before any scatter or encounter placement.
    const Cell objective = plan.rooms[static_cast<std::size_t>(plan.objective_room)].center;
    spawn_entity(game, game.run.objective == ObjectiveKind::Key ? EntityKind::Key : EntityKind::Switch, objective);
    spawn_entity(game, EntityKind::Door, plan.door);
    spawn_entity(game, EntityKind::Exit, game.run.exit);
    if (forest_floor(game.run.floor))
        place_ground_item(game,game.run.spawn+Cell{0,2},ItemKind::Stick);
    else {
        // Reserve one ordinary equipment slot for a native combat tool before
        // situational role supplies can consume the floor's entire budget.
        const ItemKind weapon=roll_item_supply(game,LootSource::Weapon,false);
        if (weapon!=ItemKind::None) {
            place_ground_item(game,game.run.spawn+Cell{0,2},weapon,supply_count(weapon));
            --budget.equipment;
        }
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::AshLoft && budget.threat>=2 && budget.equipment>=2) {
        if (populate_ash_loft(game,plan,room)) {budget.threat-=2;budget.equipment-=2;}
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::SlagBank && budget.threat>=2 && budget.equipment>0) {
        if (populate_slag_bank(game,plan,room)) {budget.threat-=2;--budget.equipment;}
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::LampAlcove && budget.threat>=2 && budget.equipment>0) {
        if (populate_lamp_alcove(game,plan,room)) {budget.threat-=2;--budget.equipment;}
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::PayOffice && budget.threat>=3) {
        if (populate_pay_office(game,plan,room)) budget.threat-=3;
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::KilnCourt && budget.threat>=3) {
        if (populate_kiln_court(game,plan,room)) {budget.threat-=3;budget.equipment=std::max(0,budget.equipment-1);}
        else enemy(game,room,EntityKind::WalkingKiln,3,budget);
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::CableTrench && budget.threat>=2) {
        if (populate_cable_trench(game,plan,room)) {budget.threat-=2;budget.equipment=std::max(0,budget.equipment-1);}
        else enemy(game,room,EntityKind::CableCrawler,2,budget);
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::CoolingWorks && budget.threat>=2) {
        if (populate_cooling_works(game,plan,room)) {budget.threat-=2;budget.equipment=std::max(0,budget.equipment-1);}
        else enemy(game,room,EntityKind::PressureRat,1,budget);
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::RepairBay && budget.threat>=2) {
        if (get_entity(game,populate_repair_bay(game,plan,room))) budget.threat-=2;
        else enemy(game,room,EntityKind::ArcWelder,2,budget);
    }
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::ScrapYard && budget.threat>=2) {
        if (get_entity(game,populate_scrap_yard(game,plan,room))) {budget.threat-=2;budget.equipment=std::max(0,budget.equipment-2);}
        else enemy(game,room,EntityKind::MagnetCrane,2,budget);
    }
    // Reserve crew budget before incidental encounters consume it.
    for (const RoomPlan& room:plan.rooms)
        if (room.role==RoomRole::Workfront && budget.threat>=5) {
            const int crew=populate_workfront(game,room);
            if (crew>0) budget.threat-=crew>=5 ? 8 : 5;
        }
    bool assembly=false;
    for (const RoomPlan& room:plan.rooms) if (room.role==RoomRole::AssemblyLine) {
        assembly=true; assembly_supplies(game,room);
        budget.equipment=std::max(0,budget.equipment-2);
        if (budget.threat>=2) {
            if (populate_rivet_post(game,plan,room)) budget.threat-=2;
            else enemy(game,room,EntityKind::RivetGunner,2,budget);
        }
    }
    for (const RoomPlan& room:plan.rooms)
        if (room.role==RoomRole::BlastingAlcove) {
            enemy(game,room,EntityKind::PowderMonkey,2,budget);
            if (!assembly && budget.threat>=2) {
                if (populate_rivet_post(game,plan,room)) budget.threat-=2;
                else enemy(game,room,EntityKind::RivetGunner,2,budget);
            }
            if (const auto cell=room_space(game,room)) place_ground_item(game,*cell,ItemKind::FuseScissors);
        }
    bool salvage=false;
    for (const RoomPlan& room : plan.rooms) {
        room_light(game, room);
        if (room.role == RoomRole::Entrance || room.role == RoomRole::Exit) continue;
        place_sluice_chamber(game,plan,room);
        if (!salvage) salvage=place_salvage_pocket(game,plan,room);
        encounter(game, plan, room, budget);
        place_crystal_vein(game,plan,room);
        room_loot(game, room, budget);
    }
    // SUPPLIES: A sparse role roll must not accidentally remove all healing or new equipment.
    const RoomPlan& shrine = plan.rooms[static_cast<std::size_t>(plan.objective_room)];
    while (budget.healing > 0) {
        const int before = budget.healing;
        supply(game, shrine, ItemKind::Bandage, 2, budget.healing);
        if (before == budget.healing) break;
    }
    if (budget.equipment > 0) {
        const ItemKind weapon=roll_item_supply(game,LootSource::Weapon,false);
        if (weapon!=ItemKind::None) supply(game,shrine,weapon,supply_count(weapon),budget.equipment);
    }
    if (budget.ammunition > 0) supply(game, shrine, ItemKind::Ammo, 1, budget.ammunition);
    while (budget.stashes > 0) {
        const int before = budget.stashes;
        stash(game, shrine, budget);
        if (before == budget.stashes) break;
    }
}
