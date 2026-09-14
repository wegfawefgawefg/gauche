#include "loot.hpp"
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
    case EntityKind::Zombie: case EntityKind::ZombieStack:
        if (random_u32(game) % 3 == 0)
            place_coins(game, enemy.cell, 2 + static_cast<int>(random_u32(game) % 5));
        break;
    case EntityKind::Chicken: case EntityKind::Bunny:
        if (random_u32(game) % 10 == 0) place_ground_item(game, enemy.cell, ItemKind::RawMeat);
        break;
    case EntityKind::RootTurret:
        if (random_u32(game) % 5 == 0) place_ground_item(game, enemy.cell, ItemKind::Stick);
        break;
    case EntityKind::BrambleGuard:
        if (random_u32(game) % 100 < 15) place_ground_item(game, enemy.cell, ItemKind::SapJar);
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
