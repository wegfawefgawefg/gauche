#include "loot.hpp"
#include "../entities/scavenging.hpp"
#include "../entities/shard_colony.hpp"
#include "ground_items.hpp"

#include <algorithm>

void place_coins(Game& game, Cell cell, int amount) {
    if (amount <= 0) return;
    const Cell destination = nearby_ground_item_cell(game, cell);
    for (Entity& pile : game.entities)
        if (pile.kind == EntityKind::Coins && pile.cell == destination) {
            pile.counter_a += amount;
            return;
        }
    Entity* pile = get_entity(game, spawn_entity(game, EntityKind::Coins, destination));
    if (pile != nullptr) pile->counter_a = amount;
}

void collect_coins(Game& game, Entity& player) {
    if (player.owner < 0 || player.owner >= 4 || player.health <= 0) return;
    for (int slot = 0; slot < max_entities; ++slot) {
        Entity& pile = game.entities[static_cast<std::size_t>(slot)];
        if (pile.kind != EntityKind::Coins || pile.cell != player.cell) continue;
        game.run.coins[static_cast<std::size_t>(player.owner)] += pile.counter_a;
        remove_entity(game, {slot, pile.generation});
        emit_sound(game, SoundId::CoinPickup, player.cell);
    }
}

void drop_enemy_loot(Game& game, const Entity& enemy) {
    // POCKETS: Money comes from plausible carriers and caches, not every animal kill.
    switch (enemy.kind) {
    case EntityKind::FurnaceMoth: {
        const auto roll=random_u32(game)%100;
        if (roll>=25 && roll<35) place_ground_item(game,enemy.cell,ItemKind::HeatCapsule);
        break; // The catalog glow-slag range remains empty until implemented.
    }
    case EntityKind::AuditClerk: {
        place_coins(game,enemy.cell,enemy.counter_a);
        const auto roll=random_u32(game)%100;
        if (roll>=40 && roll<65) place_coins(game,enemy.cell,3+static_cast<int>(random_u32(game)%5));
        break; // Punch card and inspector stamp remain unimplemented catalog drops.
    }
    case EntityKind::WalkingKiln: {
        const auto roll=random_u32(game)%100;
        // Plate/biscuit catalog entries are still candidates, not substitute loot.
        if (roll>=45 && roll<60 && enemy.counter_a>0)
            place_ground_item(game,enemy.cell,ItemKind::CoalLump,std::min(2,enemy.counter_a));
        break;
    }
    case EntityKind::CableCrawler:
        if (random_u32(game)%100<25) place_ground_item(game,enemy.cell,ItemKind::CopperWire);
        break; // Insulated boots await their actual item implementation.
    case EntityKind::PressureRat:
        if (random_u32(game)%100<25) place_ground_item(game,enemy.cell,ItemKind::RawMeat);
        break; // Rubber hose reserved until its item is implemented.
    case EntityKind::RivetGunner: {
        const auto roll=random_u32(game)%100;
        if (roll<25) place_ground_item(game,enemy.cell,ItemKind::RivetGun);
        else if (roll<45) place_ground_item(game,enemy.cell,ItemKind::Ammo);
        break;
    }
    case EntityKind::Strikebreaker: {
        const auto roll=random_u32(game)%100;
        if (roll<20) place_ground_item(game,enemy.cell,ItemKind::PressHammer);
        else if (roll>=35 && roll<55) place_coins(game,enemy.cell,5+static_cast<int>(random_u32(game)%5));
        break;
    }
    case EntityKind::PowderMonkey: {
        const auto roll=random_u32(game)%100;
        if (roll<25 && enemy.counter_a>0) place_ground_item(game,enemy.cell,ItemKind::QuarryCharge);
        else if (roll>=25 && roll<40) place_ground_item(game,enemy.cell,ItemKind::FuseScissors);
        break;
    }
    case EntityKind::Ember:
        if (random_u32(game)%100<25 && enemy.counter_a+enemy.counter_b>0)
            place_ground_item(game,enemy.cell,ItemKind::CoalLump,std::min(2,enemy.counter_a+enemy.counter_b));
        break;
    case EntityKind::Pickhand: {
        const auto roll=random_u32(game)%100;
        if (roll<20) {
            const Handle handle=spawn_entity(game,EntityKind::GroundItem,nearby_ground_item_cell(game,enemy.cell));
            if (Entity* item=get_entity(game,handle)) {
                item->ground_item=make_item(ItemKind::Pickaxe);
                item->ground_item.uses=12; item->sprite=Sprite::Pickaxe;
            }
        } else if (roll<40) place_coins(game,enemy.cell,3+static_cast<int>(random_u32(game)%4));
        break;
    }
    case EntityKind::ShiftForeman: {
        const auto roll=random_u32(game)%100;
        if (roll<25) place_ground_item(game,enemy.cell,ItemKind::ForemanWhistle);
        else if (roll<50) place_coins(game,enemy.cell,5+static_cast<int>(random_u32(game)%5));
        break;
    }
    case EntityKind::BoilerPorter: {
        const auto roll = random_u32(game)%100;
        if (roll < 25) place_ground_item(game,enemy.cell,ItemKind::PressureValve);
        else if (roll < 45) place_ground_item(game,enemy.cell,ItemKind::CoalLump,2);
        break;
    }
    case EntityKind::BoilerTank:
        if (enemy.ground_item.kind == ItemKind::PressureValve)
            place_ground_item(game,enemy.cell,ItemKind::PressureValve);
        break;
    case EntityKind::IcicleSpider: {
        const auto roll = random_u32(game)%100;
        if (roll < 20) place_ground_item(game,enemy.cell,ItemKind::FishingLine);
        else if (roll < 30) place_ground_item(game,enemy.cell,ItemKind::IceNeedle);
        break;
    }
    case EntityKind::ShardColony: drop_shard_colony(game,enemy); break;
    case EntityKind::CandleKeeper: {
        const auto roll = random_u32(game) % 100;
        if (roll < 30) place_ground_item(game,enemy.cell,ItemKind::CandleStub);
        else if (roll < 45) place_ground_item(game,enemy.cell,ItemKind::WickSpool);
        break;
    }
    case EntityKind::SnowEffigy: {
        const auto roll=random_u32(game)%100;
        if (roll<20) place_ground_item(game,enemy.cell,ItemKind::CandleStub);
        else if (roll<30) place_ground_item(game,enemy.cell,ItemKind::EffigyMask);
        break;
    }
    case EntityKind::MagnetCrane: {
        const auto roll=random_u32(game)%100;
        if (roll<25) place_ground_item(game,enemy.cell,ItemKind::HorseshoeMagnet);
        else if (roll<45) place_ground_item(game,enemy.cell,ItemKind::CopperWire);
        break;
    }
    case EntityKind::ArcWelder: {
        const auto roll=random_u32(game)%100;
        if (roll<25) place_ground_item(game,enemy.cell,ItemKind::ArcTorch);
        else if (roll<40) place_ground_item(game,enemy.cell,ItemKind::CopperWire);
        break;
    }
    case EntityKind::Yeti:
        place_ground_item(game,enemy.cell,ItemKind::RawMeat,2);
        if (random_u32(game)%4==0) place_ground_item(game,enemy.cell,ItemKind::Crampons);
        break;
    case EntityKind::AvalancheRam: {
        const auto roll = random_u32(game) % 100;
        if (roll < 35) place_ground_item(game, enemy.cell, ItemKind::RawMeat, 2);
        else if (roll < 45) place_ground_item(game, enemy.cell, ItemKind::WoolWrap);
        break;
    }
    case EntityKind::WhiteoutDrummer: {
        const auto roll=random_u32(game)%100;
        if (roll<20) place_ground_item(game,enemy.cell,ItemKind::MufflingFelt);
        else if (roll<30) place_ground_item(game,enemy.cell,ItemKind::SignalFlare);
        break;
    }
    case EntityKind::SealThief:
        drop_scavenged_items(game, enemy);
        if (random_u32(game) % 4 == 0) place_ground_item(game, enemy.cell, ItemKind::RawMeat);
        break;
    case EntityKind::FishingWidow: {
        const auto roll = random_u32(game) % 100;
        if (roll < 20) place_ground_item(game, enemy.cell, ItemKind::FishingLine);
        else if (roll < 40) place_ground_item(game, enemy.cell, ItemKind::SmokedFish);
        else if (roll < 45) place_ground_item(game, enemy.cell, ItemKind::HarpoonGun);
        break;
    }
    case EntityKind::FrozenPilgrim: {
        const auto roll = random_u32(game) % 100;
        if (roll < 25) place_coins(game, enemy.cell, 2 + static_cast<int>(random_u32(game) % 4));
        else if (roll < 40) place_ground_item(game, enemy.cell, ItemKind::HotBroth);
        break;
    }
    case EntityKind::EchoHound: {
        const auto roll = random_u32(game) % 100;
        if (roll < 25) place_ground_item(game, enemy.cell, ItemKind::RawMeat);
        else if (roll < 35) place_ground_item(game, enemy.cell, ItemKind::MufflingFelt);
        else if (roll < 45) place_ground_item(game, enemy.cell, ItemKind::EchoPebble);
        break;
    }
    case EntityKind::LensWarden:
        if (random_u32(game) % 100 < 35) place_ground_item(game, enemy.cell, ItemKind::LensCarbine);
        else place_coins(game, enemy.cell, 4 + static_cast<int>(random_u32(game) % 5));
        break;
    case EntityKind::MirrorKnight: {
        const unsigned int roll = random_u32(game) % 100;
        if (roll < 20) place_ground_item(game, enemy.cell, ItemKind::MirrorShard);
        else if (roll < 40) place_coins(game, enemy.cell, 3 + static_cast<int>(random_u32(game) % 4));
        break;
    }
    case EntityKind::SnowBurrower: {
        const unsigned int roll = random_u32(game) % 100;
        if (roll < 25) place_ground_item(game, enemy.cell, ItemKind::RawMeat);
        else if (roll < 40) place_ground_item(game, enemy.cell, ItemKind::SnowScoop);
        break;
    }
    case EntityKind::GlassEel: {
        const unsigned int roll = random_u32(game) % 100;
        if (roll < 20) place_ground_item(game, enemy.cell, ItemKind::EelBattery);
        else if (roll < 35) place_ground_item(game, enemy.cell, ItemKind::RawMeat);
        break;
    }
    case EntityKind::IceMason: {
        const unsigned int roll = random_u32(game) % 100;
        if (roll < 20) place_ground_item(game, enemy.cell, ItemKind::IceBrick);
        else if (roll < 35) place_ground_item(game, enemy.cell, ItemKind::Chisel);
        else if (roll < 60) place_coins(game, enemy.cell, 2 + static_cast<int>(random_u32(game) % 3));
        else if (roll < 70) place_ground_item(game, enemy.cell, ItemKind::IceAnchor);
        break;
    }
    case EntityKind::SteamLeech: {
        const auto roll = random_u32(game)%100;
        if (roll < 20) place_ground_item(game,enemy.cell,ItemKind::HeatCapsule);
        else if (roll < 30) place_ground_item(game,enemy.cell,ItemKind::Sealant);
        break;
    }
    case EntityKind::BellDiver: {
        const unsigned int roll = random_u32(game) % 100;
        if (roll < 20) place_ground_item(game, enemy.cell, ItemKind::AirBladder);
        else if (roll < 45) place_coins(game, enemy.cell, 2 + static_cast<int>(random_u32(game) % 4));
        else if (roll < 55) place_ground_item(game,enemy.cell,ItemKind::StillwaterBell);
        break;
    }
    case EntityKind::FrostBat:
        if (random_u32(game) % 100 < 15) place_ground_item(game, enemy.cell, ItemKind::IceNeedle);
        break;
    case EntityKind::RimeSkater: {
        const auto roll=random_u32(game)%100;
        if (roll<20) place_ground_item(game,enemy.cell,ItemKind::GritPouch);
        else if (roll<30) place_ground_item(game,enemy.cell,ItemKind::SkateBlade);
        break;
    }
    case EntityKind::ForagerGoblin:
        drop_scavenged_items(game, enemy);
        if (random_u32(game) % 2 == 0) place_coins(game, enemy.cell, 3 + static_cast<int>(random_u32(game) % 5));
        break;
    case EntityKind::CarrionCrow:
        drop_scavenged_items(game, enemy);
        break;
    case EntityKind::BurrowWorm:
        if (enemy.label_a == 0 && random_u32(game) % 5 == 0)
            place_ground_item(game, enemy.cell, ItemKind::BitterRoot);
        break;
    case EntityKind::WaspNest:
        if (random_u32(game) % 100 < 40) place_ground_item(game, enemy.cell, ItemKind::HoneyPot);
        break;
    case EntityKind::Wasp:
        if (random_u32(game) % 20 == 0) place_ground_item(game, enemy.cell, ItemKind::HoneyPot);
        break;
    case EntityKind::Zombie: case EntityKind::ZombieStack:
        if (random_u32(game) % 3 == 0)
            place_coins(game, enemy.cell, 2 + static_cast<int>(random_u32(game) % 5));
        break;
    case EntityKind::Chicken: case EntityKind::Bunny:
        if (random_u32(game) % 10 == 0) place_ground_item(game, enemy.cell, ItemKind::RawMeat);
        break;
    case EntityKind::Bat:
        if (random_u32(game) % 10 == 0) place_ground_item(game, enemy.cell, ItemKind::BitterRoot);
        break;
    case EntityKind::Mosquito:
        if (random_u32(game) % 10 == 0) place_ground_item(game, enemy.cell, ItemKind::WaterFlask);
        break;
    case EntityKind::ThornSnail:
        if (random_u32(game) % 100 < 15) place_ground_item(game, enemy.cell, ItemKind::ThornCaltrops);
        break;
    case EntityKind::Owl:
        if (random_u32(game) % 100 < 15) place_ground_item(game, enemy.cell, ItemKind::BirdSeed);
        break;
    case EntityKind::Woodpecker:
        if (random_u32(game) % 100 < 15) place_ground_item(game, enemy.cell, ItemKind::DiggingClaws);
        break;
    case EntityKind::RootTurret:
        if (random_u32(game) % 5 == 0) place_ground_item(game, enemy.cell, ItemKind::SeedBag);
        break;
    case EntityKind::BrambleGuard:
        if (random_u32(game) % 100 < 15) place_ground_item(game, enemy.cell, ItemKind::ResinGlue);
        break;
    case EntityKind::LanternMoth:
        if (random_u32(game) % 100 < 15) place_ground_item(game, enemy.cell, ItemKind::LanternSeed);
        break;
    case EntityKind::SporeToad:
        if (random_u32(game) % 4 == 0) place_ground_item(game, enemy.cell, ItemKind::MushroomSpores);
        break;
    case EntityKind::Boar:
        if (random_u32(game) % 100 < 35) place_ground_item(game, enemy.cell, ItemKind::RawMeat);
        break;
    case EntityKind::CrateMimic:
        if (random_u32(game) % 2 == 0) {
            constexpr ItemKind stolen[]{ItemKind::Bandage, ItemKind::Ammo, ItemKind::Pickaxe, ItemKind::BearTrap};
            place_ground_item(game, enemy.cell, stolen[random_u32(game) % std::size(stolen)]);
        }
        break;
    case EntityKind::Wolf:
        if (random_u32(game) % 5 == 0) place_ground_item(game, enemy.cell, ItemKind::RawMeat);
        break;
    case EntityKind::Bear: case EntityKind::Den:
        if (random_u32(game) % 2 == 0) place_ground_item(game, enemy.cell, ItemKind::RawMeat, 2);
        break;
    case EntityKind::Spawner:
        if (random_u32(game) % 3 == 0) place_ground_item(game, enemy.cell, ItemKind::Ammo);
        break;
    default: break;
    }
}
