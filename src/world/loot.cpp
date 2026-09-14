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
    case EntityKind::ShardColony: drop_shard_colony(game,enemy); break;
    case EntityKind::CandleKeeper: {
        const auto roll = random_u32(game) % 100;
        if (roll < 30) place_ground_item(game,enemy.cell,ItemKind::CandleStub);
        else if (roll < 45) place_ground_item(game,enemy.cell,ItemKind::WickSpool);
        break;
    }
    case EntityKind::SnowEffigy:
        if (random_u32(game) % 100 < 20) place_ground_item(game, enemy.cell, ItemKind::CandleStub);
        break;
    case EntityKind::AvalancheRam: {
        const auto roll = random_u32(game) % 100;
        if (roll < 35) place_ground_item(game, enemy.cell, ItemKind::RawMeat, 2);
        else if (roll < 45) place_ground_item(game, enemy.cell, ItemKind::WoolWrap);
        break;
    }
    case EntityKind::WhiteoutDrummer:
        if (random_u32(game) % 5 == 0) place_ground_item(game, enemy.cell, ItemKind::MufflingFelt);
        break;
    case EntityKind::SealThief:
        drop_scavenged_items(game, enemy);
        if (random_u32(game) % 4 == 0) place_ground_item(game, enemy.cell, ItemKind::RawMeat);
        break;
    case EntityKind::FishingWidow: {
        const auto roll = random_u32(game) % 100;
        if (roll < 20) place_ground_item(game, enemy.cell, ItemKind::FishingLine);
        else if (roll < 40) place_ground_item(game, enemy.cell, ItemKind::SmokedFish);
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
        break;
    }
    case EntityKind::SteamLeech:
        if (random_u32(game) % 5 == 0) place_ground_item(game, enemy.cell, ItemKind::HeatCapsule);
        break;
    case EntityKind::BellDiver: {
        const unsigned int roll = random_u32(game) % 100;
        if (roll < 20) place_ground_item(game, enemy.cell, ItemKind::AirBladder);
        else if (roll < 45) place_coins(game, enemy.cell, 2 + static_cast<int>(random_u32(game) % 4));
        break;
    }
    case EntityKind::FrostBat:
        if (random_u32(game) % 100 < 15) place_ground_item(game, enemy.cell, ItemKind::IceNeedle);
        break;
    case EntityKind::RimeSkater:
        if (random_u32(game) % 5 == 0) place_ground_item(game, enemy.cell, ItemKind::GritPouch);
        break;
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
