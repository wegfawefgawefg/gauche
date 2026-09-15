#include "ash_sleeper.hpp"
#include "slag_snail.hpp"
#include "furnace_moth.hpp"
#include "audit_clerk.hpp"
#include "walking_kiln.hpp"
#include "cable_crawler.hpp"
#include "pressure_rat.hpp"
#include "magnet_crane.hpp"
#include "arc_welder.hpp"
#include "yeti.hpp"
#include "strikebreaker.hpp"
#include "rivet_gunner.hpp"
#include "mine_crew.hpp"
#include "powder_monkey.hpp"
#include "../items/sled.hpp"
#include "../items/ice_anchor.hpp"
#include "dispatch.hpp"
#include "gate.hpp"
#include "../world/floating_items.hpp"
#include "pocket_door.hpp"
#include "rime_skater.hpp"
#include "frost_bat.hpp"
#include "bell_diver.hpp"
#include "steam_leech.hpp"
#include "ice_mason.hpp"
#include "glass_eel.hpp"
#include "snow_burrower.hpp"
#include "mirror_knight.hpp"
#include "lens_warden.hpp"
#include "echo_hound.hpp"
#include "frozen_pilgrim.hpp"
#include "fishing_widow.hpp"
#include "seal_thief.hpp"
#include "whiteout_drummer.hpp"
#include "avalanche_ram.hpp"
#include "snow_effigy.hpp"
#include "candle_keeper.hpp"
#include "shard_colony.hpp"
#include "icicle_spider.hpp"
#include "boiler_porter.hpp"
#include "boiler_tank.hpp"
#include "../projectiles/projectile.hpp"
#include "../world/encounter.hpp"

void init_entity(Game& game, Entity& entity) {
    // Each kind owns its setup beside its step. Dispatch stays deliberately plain.
    switch (entity.kind) {
    case EntityKind::RivetGunner: init_rivet_gunner(entity); break;
    case EntityKind::Strikebreaker: init_strikebreaker(entity); break;
    case EntityKind::PowderMonkey: init_powder_monkey(entity); break;
    case EntityKind::Pickhand: case EntityKind::ShiftForeman: init_mine_worker(entity); break;
    case EntityKind::BoilerPorter: init_boiler_porter(entity); break;
    case EntityKind::BoilerTank: init_boiler_tank(entity); break;
    case EntityKind::IcicleSpider: init_icicle_spider(entity); break;
    case EntityKind::ShardColony: init_shard_node(game,entity); break;
    case EntityKind::CandleKeeper: init_candle_keeper(entity); break;
    case EntityKind::SnowEffigy: init_snow_effigy(entity); break;
    case EntityKind::AshSleeper: init_ash_sleeper(entity); break;
    case EntityKind::SlagSnail: init_slag_snail(entity); break;
    case EntityKind::FurnaceMoth: init_furnace_moth(entity); break;
    case EntityKind::AuditClerk: init_audit_clerk(entity); break;
    case EntityKind::WalkingKiln: init_walking_kiln(entity); break;
    case EntityKind::CableCrawler: init_cable_crawler(entity); break;
    case EntityKind::PressureRat: init_pressure_rat(entity); break;
    case EntityKind::MagnetCrane: init_magnet_crane(entity); break;
    case EntityKind::ArcWelder: init_arc_welder(entity); break;
    case EntityKind::Yeti: init_yeti(entity); break;
    case EntityKind::AvalancheRam: init_avalanche_ram(entity); break;
    case EntityKind::WhiteoutDrummer: init_whiteout_drummer(entity); break;
    case EntityKind::SealThief: init_seal_thief(entity); break;
    case EntityKind::FishingWidow: init_fishing_widow(entity); break;
    case EntityKind::FrozenPilgrim: init_frozen_pilgrim(entity); break;
    case EntityKind::EchoHound: init_echo_hound(entity); break;
    case EntityKind::LensWarden: init_lens_warden(entity); break;
    case EntityKind::MirrorKnight: init_mirror_knight(entity); break;
    case EntityKind::SnowBurrower: init_snow_burrower(entity); break;
    case EntityKind::GlassEel: init_glass_eel(entity); break;
    case EntityKind::IceMason: init_ice_mason(entity); break;
    case EntityKind::SteamLeech: init_steam_leech(entity); break;
    case EntityKind::BellDiver: init_bell_diver(entity); break;
    case EntityKind::RimeSkater: init_rime_skater(entity); break;
    case EntityKind::PocketDoor: init_pocket_door(entity); break;
    case EntityKind::Sled: init_sled(entity); break;
    case EntityKind::IceAnchor: init_ice_anchor(entity); break;
    case EntityKind::WaspNest: init_wasp_nest(entity); break;
    case EntityKind::Wasp: init_wasp(entity); break;
    case EntityKind::ForagerGoblin: init_forager_goblin(game, entity); break;
    case EntityKind::CarrionCrow: init_carrion_crow(entity); break;
    case EntityKind::BurrowWorm: init_burrow_worm(entity); break;
    case EntityKind::Projectile: init_projectile(entity); break;
    case EntityKind::Boar: init_boar(entity); break;
    case EntityKind::ThornSnail: init_thorn_snail(entity); break;
    case EntityKind::LanternMoth: init_lantern_moth(entity); break;
    case EntityKind::Mosquito: init_mosquito(entity); break;
    case EntityKind::Owl: init_owl(entity); break;
    case EntityKind::Woodpecker: init_woodpecker(entity); break;
    case EntityKind::RootTurret: init_root_turret(entity); break;
    case EntityKind::BrambleGuard: init_bramble_guard(entity); break;
    case EntityKind::SporeToad: init_spore_toad(entity); break;
    case EntityKind::CrateMimic: init_crate_mimic(entity); break;
    case EntityKind::Encounter: init_encounter(entity); break;
    case EntityKind::Player: init_player(entity); break;
    case EntityKind::ZombieStack: init_zombie_stack(game, entity); break;
    case EntityKind::Zombie: init_zombie(game, entity); break;
    case EntityKind::Chicken: init_chicken(game, entity); break;
    case EntityKind::Bat: init_bat(entity); break;
    case EntityKind::FrostBat: init_frost_bat(entity); break;
    case EntityKind::Wolf: init_wolf(entity); break;
    case EntityKind::Dog: init_dog(entity); break;
    case EntityKind::Bear: init_bear(entity); break;
    case EntityKind::Bunny: init_bunny(entity); break;
    case EntityKind::Ember: init_ember(entity); break;
    case EntityKind::Spawner: init_spawner(entity); break;
    case EntityKind::Den: init_den(entity); break;
    case EntityKind::RailLayer: init_rail(entity); break;
    case EntityKind::Train: init_train(entity); break;
    case EntityKind::Key: case EntityKind::Door: case EntityKind::Exit:
    case EntityKind::Trap: case EntityKind::Switch: case EntityKind::Campfire:
    case EntityKind::Coins: case EntityKind::WaveVent: case EntityKind::EncounterGate:
    case EntityKind::Crusher: init_fixture(entity); break;
    case EntityKind::GroundItem: case EntityKind::None: case EntityKind::Count: break;
    }
}

void step_entity(Game& game, int slot) {
    switch (game.entities[static_cast<std::size_t>(slot)].kind) {
    case EntityKind::RivetGunner: step_rivet_gunner(game,slot); break;
    case EntityKind::Strikebreaker: step_strikebreaker(game,slot); break;
    case EntityKind::PowderMonkey: step_powder_monkey(game,slot); break;
    case EntityKind::Pickhand: case EntityKind::ShiftForeman: step_mine_worker(game,slot); break;
    case EntityKind::IceAnchor: step_ice_anchor(game,slot); break;
    case EntityKind::Sled: step_sled(game,slot); break;
    case EntityKind::GroundItem: step_sled_cargo(game,slot); step_floating_item(game, slot); break;
    case EntityKind::BoilerPorter: step_boiler_porter(game,slot); break;
    case EntityKind::BoilerTank: break; // Pressure runs with timers, including during stun.
    case EntityKind::IcicleSpider: step_icicle_spider(game,slot); break;
    case EntityKind::ShardColony: step_shard_colony(game,slot); break;
    case EntityKind::CandleKeeper: step_candle_keeper(game,slot); break;
    case EntityKind::SnowEffigy: step_snow_effigy(game, slot); break;
    case EntityKind::AshSleeper: step_ash_sleeper(game,slot); break;
    case EntityKind::SlagSnail: step_slag_snail(game,slot); break;
    case EntityKind::FurnaceMoth: step_furnace_moth(game,slot); break;
    case EntityKind::AuditClerk: step_audit_clerk(game,slot); break;
    case EntityKind::WalkingKiln: step_walking_kiln(game,slot); break;
    case EntityKind::CableCrawler: step_cable_crawler(game,slot); break;
    case EntityKind::PressureRat: step_pressure_rat(game,slot); break;
    case EntityKind::MagnetCrane: step_magnet_crane(game,slot); break;
    case EntityKind::ArcWelder: step_arc_welder(game,slot); break;
    case EntityKind::Yeti: step_yeti(game,slot); break;
    case EntityKind::AvalancheRam: step_avalanche_ram(game, slot); break;
    case EntityKind::WhiteoutDrummer: step_whiteout_drummer(game, slot); break;
    case EntityKind::SealThief: step_seal_thief(game, slot); break;
    case EntityKind::FishingWidow: step_fishing_widow(game, slot); break;
    case EntityKind::FrozenPilgrim: step_frozen_pilgrim(game, slot); break;
    case EntityKind::EchoHound: step_echo_hound(game, slot); break;
    case EntityKind::LensWarden: step_lens_warden(game, slot); break;
    case EntityKind::MirrorKnight: step_mirror_knight(game, slot); break;
    case EntityKind::SnowBurrower: step_snow_burrower(game, slot); break;
    case EntityKind::GlassEel: step_glass_eel(game, slot); break;
    case EntityKind::IceMason: step_ice_mason(game, slot); break;
    case EntityKind::SteamLeech: step_steam_leech(game, slot); break;
    case EntityKind::BellDiver: step_bell_diver(game, slot); break;
    case EntityKind::RimeSkater: step_rime_skater(game, slot); break;
    case EntityKind::WaspNest: step_wasp_nest(game, slot); break;
    case EntityKind::Wasp: step_wasp(game, slot); break;
    case EntityKind::ForagerGoblin: step_forager_goblin(game, slot); break;
    case EntityKind::CarrionCrow: step_carrion_crow(game, slot); break;
    case EntityKind::BurrowWorm: step_burrow_worm(game, slot); break;
    case EntityKind::Projectile: step_projectile(game, slot); break;
    case EntityKind::Boar: step_boar(game, slot); break;
    case EntityKind::ThornSnail: step_thorn_snail(game, slot); break;
    case EntityKind::LanternMoth: step_lantern_moth(game, slot); break;
    case EntityKind::Mosquito: step_mosquito(game, slot); break;
    case EntityKind::Owl: step_owl(game, slot); break;
    case EntityKind::Woodpecker: step_woodpecker(game, slot); break;
    case EntityKind::RootTurret: step_root_turret(game, slot); break;
    case EntityKind::BrambleGuard: step_bramble_guard(game, slot); break;
    case EntityKind::SporeToad: step_spore_toad(game, slot); break;
    case EntityKind::CrateMimic: step_crate_mimic(game, slot); break;
    case EntityKind::EncounterGate: step_gate(game,slot); break;
    case EntityKind::Encounter: step_encounter(game, slot); break;
    case EntityKind::ZombieStack: step_zombie_stack(game, slot); break;
    case EntityKind::Zombie: step_zombie(game, slot); break;
    case EntityKind::Chicken: step_chicken(game, slot); break;
    case EntityKind::Bat: step_bat(game, slot); break;
    case EntityKind::FrostBat: step_frost_bat(game, slot); break;
    case EntityKind::Wolf: step_wolf(game, slot); break;
    case EntityKind::Dog: step_dog(game, slot); break;
    case EntityKind::Bear: step_bear(game, slot); break;
    case EntityKind::Bunny: step_bunny(game, slot); break;
    case EntityKind::Ember: step_ember(game, slot); break;
    case EntityKind::Spawner: step_spawner(game, slot); break;
    case EntityKind::Den: step_den(game, slot); break;
    case EntityKind::RailLayer: step_rail(game, slot); break;
    case EntityKind::Train: step_train(game, slot); break;
    default: break;
    }
}
