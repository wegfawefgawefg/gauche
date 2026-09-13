#include "game.hpp"

#include <algorithm>
#include <cstdint>

namespace {

constexpr std::array<Cell, 4> neighbors{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

void pickup_item(Game& game, Entity& player) {
    for (int slot = 0; slot < max_entities; ++slot) {
        Entity& ground = game.entities[static_cast<std::size_t>(slot)];
        if (ground.kind != EntityKind::GroundItem || ground.cell != player.cell) continue;
        Inventory result = player.inventory;
        if (!insert_item(result, ground.ground_item)) return;
        player.inventory = result;
        remove_entity(game, {slot, ground.generation});
        return;
    }
}

void drop_item(Game& game, Entity& player) {
    Item& item = *player.inventory.held();
    if (item.kind == ItemKind::None || item.kind == ItemKind::Fist) return;
    for (const Entity& entity : game.entities) {
        if (entity.kind == EntityKind::GroundItem && entity.cell == player.cell) return;
    }
    const Handle dropped = spawn_entity(game, EntityKind::GroundItem, player.cell);
    if (Entity* entity = get_entity(game, dropped)) {
        entity->ground_item = item;
        entity->sprite = item_sprite(item.kind);
        item = {};
    }
}

void step_player(Game& game, int slot, const Input& input) {
    Entity& player = game.entities[static_cast<std::size_t>(slot)];
    if (input.select >= 0 && input.select < quick_slots) player.inventory.selected = input.select;
    if (player.move_wait == 0 && (input.move.x != 0 || input.move.y != 0)) {
        Cell movement = input.move;
        if (movement.x != 0) movement.y = 0;
        move_entity(game, slot, player.cell + movement);
    }
    if (input.aim.x != 0 || input.aim.y != 0)
        player.facing = input.aim;
    if (input.pickup) pickup_item(game, player);
    if (input.interact) {
        if (!interact_with_fixture(game, player.owner, player.cell))
            interact_with_fixture(game, player.owner, player.cell + player.facing);
    }
    if (input.drop) drop_item(game, player);
    if (input.reload) reload_held_item(game, slot);
    if (input.use) {
        const Cell target = player.cell + input.aim;
        if (!interact_with_fixture(game, player.owner, target))
            use_held_item(game, slot, target);
    }
}

void wander(Game& game, int slot) {
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    if (entity.move_wait > 0) return;
    const std::uint32_t choice = random_u32(game) % 5;
    if (choice < 4) move_entity(game, slot, entity.cell + neighbors[choice]);
}

void step_zombie(Game& game, int slot) {
    Entity& zombie = game.entities[static_cast<std::size_t>(slot)];
    wander(game, slot);
    zombie.sprite = Sprite::Zombie;
    if (zombie.attack_wait > 0) return;
    for (Cell direction : neighbors) {
        const Cell target_cell = zombie.cell + direction;
        for (int target_slot = 0; target_slot < max_entities; ++target_slot) {
            const Entity& target = game.entities[static_cast<std::size_t>(target_slot)];
            if (target.cell != target_cell ||
                (target.kind != EntityKind::Player && target.kind != EntityKind::Chicken)) continue;
            damage_entity(game, target_slot, 5, zombie.cell);
            zombie.facing = direction;
            zombie.attack_wait = zombie.attack_interval;
            zombie.sprite = Sprite::ZombieScratch1;
            return;
        }
    }
}

void step_rail_layer(Game& game, int slot) {
    Entity& rail = game.entities[static_cast<std::size_t>(slot)];
    const Cell next = rail.cell + rail.facing;
    if (game.stage.in_bounds(next)) {
        *game.stage.at(next) = {TileKind::Rail, 0, 0};
        rail.cell = next;
        return;
    }
    const Cell origin{game.stage.width - 1, rail.cell.y};
    for (Entity& entity : game.entities) {
        if (entity.kind != EntityKind::None && entity.kind != EntityKind::RailLayer &&
            entity.cell == origin) entity.health = 0;
    }
    const Handle train = spawn_entity(game, EntityKind::Train, origin);
    if (Entity* head = get_entity(game, train)) {
        head->train_origin = origin;
        head->train_cars_left = 5 + static_cast<int>(random_u32(game) % 16);
    }
    remove_entity(game, {slot, rail.generation});
}

void step_train(Game& game, int slot) {
    Entity& train = game.entities[static_cast<std::size_t>(slot)];
    const Cell next = train.cell + train.facing;
    const Tile* tile = game.stage.at(next);
    if (tile == nullptr || tile->kind != TileKind::Rail) {
        remove_entity(game, {slot, train.generation});
        return;
    }
    for (int other_slot = 0; other_slot < max_entities; ++other_slot) {
        if (other_slot == slot) continue;
        Entity& other = game.entities[static_cast<std::size_t>(other_slot)];
        if (other.kind == EntityKind::Train && other.cell == next) return;
        if (other.kind != EntityKind::None && other.cell == next) {
            if (other.kind == EntityKind::GroundItem)
                remove_entity(game, {other_slot, other.generation});
            else damage_entity(game, other_slot, 1000, train.cell);
        }
    }
    train.cell = next;
    if (train.train_cars_left > 0) {
        const Handle car = spawn_entity(game, EntityKind::Train, train.train_origin);
        if (Entity* entity = get_entity(game, car)) {
            entity->sprite = train.train_cars_left == 1 ? Sprite::Caboose : Sprite::TrainCarA;
            entity->train_cars_left = 0;
        }
        --train.train_cars_left;
    }
}

void step_spawner(Game& game, int slot) {
    Entity& spawner = game.entities[static_cast<std::size_t>(slot)];
    if (spawner.spawn_wait > 0) {
        --spawner.spawn_wait;
        return;
    }
    int nearby = 0;
    for (const Entity& entity : game.entities)
        if (entity.kind == EntityKind::Zombie && distance(entity.cell, spawner.cell) < 8)
            ++nearby;
    if (nearby < 4) {
        for (Cell direction : neighbors) {
            const Cell cell = spawner.cell + direction;
            const Tile* tile = game.stage.at(cell);
            if (tile != nullptr && walkable(tile->kind) && entity_at(game, cell, true) < 0) {
                spawn_entity(game, EntityKind::Zombie, cell);
                break;
            }
        }
    }
    spawner.spawn_wait = 120;
}

void mix(std::uint64_t& hash, std::uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
}

} // namespace

void step_game(Game& game, const std::array<Input, 4>& inputs) {
    if (!game.started || game.game_over) return;
    ++game.tick;
    if (game.run.phase == RunPhase::Reward) {
        for (std::size_t owner = 0; owner < game.players.size(); ++owner) {
            Entity* player = get_entity(game, game.players[owner]);
            if (player == nullptr) continue;
            if (inputs[owner].select >= 0 && inputs[owner].select < 3)
                choose_reward(game, static_cast<int>(owner), inputs[owner].select);
            else if (inputs[owner].drop) drop_item(game, *player);
        }
        return;
    }
    if (game.run.phase == RunPhase::Shop) {
        if (inputs[0].select >= 0 && inputs[0].select < 3)
            buy_shop_item(game, 0, inputs[0].select);
        if (inputs[0].confirm) advance_run(game);
        return;
    }
    if (game.run.phase == RunPhase::Won) return;
    for (Entity& entity : game.entities) {
        if (entity.kind == EntityKind::None) continue;
        entity.move_wait = std::max(0, entity.move_wait - 1);
        entity.attack_wait = std::max(0, entity.attack_wait - 1);
        entity.block_ticks = std::max(0, entity.block_ticks - 1);
        entity.use_flash = std::max(0, entity.use_flash - 1);
        if (entity.kind == EntityKind::Player && entity.health == 0 &&
            game.run.death_policy == DeathPolicy::Entrance && game.run.phase == RunPhase::Playing) {
            entity.spawn_wait = std::max(0, entity.spawn_wait - 1);
            if (entity.spawn_wait == 0) {
                entity.cell = game.run.spawn;
                entity.health = entity.max_health;
                entity.impassable = true;
                entity.sprite = Sprite::Player;
            }
        }
        for (Item& item : entity.inventory.slots) item.cooldown = std::max(0, item.cooldown - 1);
    }
    for (std::size_t owner = 0; owner < game.players.size(); ++owner) {
        const Handle handle = game.players[owner];
        Entity* player = get_entity(game, handle);
        if (player != nullptr && player->health > 0) step_player(game, handle.slot, inputs[owner]);
    }
    if (game.run.phase == RunPhase::Reward) return;
    for (int slot = 0; slot < max_entities; ++slot) {
        Entity& entity = game.entities[static_cast<std::size_t>(slot)];
        if (entity.birth_tick == game.tick ||
            (entity.health == 0 && entity.kind != EntityKind::RailLayer &&
             entity.kind != EntityKind::GroundItem)) continue;
        switch (entity.kind) {
        case EntityKind::Zombie: step_zombie(game, slot); break;
        case EntityKind::Chicken: wander(game, slot); break;
        case EntityKind::RailLayer: step_rail_layer(game, slot); break;
        case EntityKind::Train: step_train(game, slot); break;
        case EntityKind::Spawner: step_spawner(game, slot); break;
        default: break;
        }
    }
    for (int slot = 0; slot < max_entities; ++slot) {
        Entity& entity = game.entities[static_cast<std::size_t>(slot)];
        if (entity.kind != EntityKind::None && entity.kind != EntityKind::Player &&
            entity.health == 0 &&
            entity.kind != EntityKind::RailLayer && entity.kind != EntityKind::GroundItem)
            remove_entity(game, {slot, entity.generation});
    }
    bool any_alive = false;
    for (Handle handle : game.players) {
        const Entity* player = get_entity(game, handle);
        any_alive |= player != nullptr && player->health > 0;
    }
    game.game_over = !any_alive && game.run.death_policy != DeathPolicy::Entrance;
}

std::uint64_t game_hash(const Game& game) {
    std::uint64_t hash = 1469598103934665603ULL;
    mix(hash, game.rng);
    mix(hash, game.tick);
    mix(hash, static_cast<std::uint64_t>(game.started));
    mix(hash, static_cast<std::uint64_t>(game.game_over));
    mix(hash, static_cast<std::uint64_t>(game.run.phase));
    mix(hash, game.run.seed);
    mix(hash, static_cast<std::uint64_t>(game.run.floor));
    mix(hash, static_cast<std::uint64_t>(game.run.has_key));
    mix(hash, static_cast<std::uint64_t>(game.run.death_policy));
    mix(hash, static_cast<std::uint64_t>(game.run.spawn.x));
    mix(hash, static_cast<std::uint64_t>(game.run.spawn.y));
    mix(hash, static_cast<std::uint64_t>(game.run.exit.x));
    mix(hash, static_cast<std::uint64_t>(game.run.exit.y));
    for (std::size_t owner = 0; owner < game.players.size(); ++owner) {
        mix(hash, static_cast<std::uint64_t>(game.players[owner].slot));
        mix(hash, game.players[owner].generation);
        mix(hash, static_cast<std::uint64_t>(game.run.coins[owner]));
        mix(hash, static_cast<std::uint64_t>(game.run.chosen[owner]));
        for (const Reward& reward : game.run.offers[owner]) {
            mix(hash, static_cast<std::uint64_t>(reward.kind));
            mix(hash, static_cast<std::uint64_t>(reward.item));
            mix(hash, static_cast<std::uint64_t>(reward.artifact));
            mix(hash, static_cast<std::uint64_t>(reward.amount));
        }
    }
    for (ItemKind item : game.run.shop_stock) mix(hash, static_cast<std::uint64_t>(item));
    mix(hash, static_cast<std::uint64_t>(game.stage.width));
    mix(hash, static_cast<std::uint64_t>(game.stage.height));
    for (const Tile& tile : game.stage.tiles) {
        mix(hash, static_cast<std::uint64_t>(tile.kind));
        mix(hash, tile.hp);
    }
    for (const Entity& entity : game.entities) {
        mix(hash, static_cast<std::uint64_t>(entity.kind));
        mix(hash, entity.generation);
        if (entity.kind == EntityKind::None) continue;
        mix(hash, static_cast<std::uint64_t>(entity.cell.x));
        mix(hash, static_cast<std::uint64_t>(entity.cell.y));
        mix(hash, static_cast<std::uint64_t>(entity.facing.x));
        mix(hash, static_cast<std::uint64_t>(entity.facing.y));
        mix(hash, static_cast<std::uint64_t>(entity.sprite));
        mix(hash, static_cast<std::uint64_t>(entity.owner));
        mix(hash, static_cast<std::uint64_t>(entity.health));
        mix(hash, static_cast<std::uint64_t>(entity.max_health));
        mix(hash, static_cast<std::uint64_t>(entity.move_wait));
        mix(hash, static_cast<std::uint64_t>(entity.move_interval));
        mix(hash, static_cast<std::uint64_t>(entity.attack_wait));
        mix(hash, static_cast<std::uint64_t>(entity.attack_interval));
        mix(hash, static_cast<std::uint64_t>(entity.block_ticks));
        mix(hash, static_cast<std::uint64_t>(entity.train_cars_left));
        mix(hash, static_cast<std::uint64_t>(entity.train_origin.x));
        mix(hash, static_cast<std::uint64_t>(entity.train_origin.y));
        mix(hash, static_cast<std::uint64_t>(entity.spawn_wait));
        mix(hash, static_cast<std::uint64_t>(entity.birth_tick));
        mix(hash, static_cast<std::uint64_t>(entity.impassable));
        mix(hash, static_cast<std::uint64_t>(entity.hard_blocker));
        mix(hash, static_cast<std::uint64_t>(entity.fixture_open));
        mix(hash, entity.artifacts);
        mix(hash, static_cast<std::uint64_t>(entity.inventory.selected));
        for (const Item& item : entity.inventory.slots) {
            mix(hash, static_cast<std::uint64_t>(item.kind));
            mix(hash, static_cast<std::uint64_t>(item.count));
            mix(hash, static_cast<std::uint64_t>(item.cooldown));
            mix(hash, static_cast<std::uint64_t>(item.loaded));
            mix(hash, static_cast<std::uint64_t>(item.spare));
            mix(hash, static_cast<std::uint64_t>(item.durability));
        }
        const Item& ground = entity.ground_item;
        mix(hash, static_cast<std::uint64_t>(ground.kind));
        mix(hash, static_cast<std::uint64_t>(ground.count));
        mix(hash, static_cast<std::uint64_t>(ground.loaded));
        mix(hash, static_cast<std::uint64_t>(ground.spare));
        mix(hash, static_cast<std::uint64_t>(ground.durability));
    }
    return hash;
}
