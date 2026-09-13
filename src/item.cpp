#include "game.hpp"

#include <algorithm>
#include <cstdlib>

namespace {

bool is_gun(ItemKind kind) {
    return kind == ItemKind::Pistol || kind == ItemKind::Musket ||
           kind == ItemKind::Bow || kind == ItemKind::RocketLauncher;
}

int magazine_size(ItemKind kind) {
    switch (kind) {
    case ItemKind::Pistol: return 12;
    case ItemKind::Musket: case ItemKind::Bow: case ItemKind::RocketLauncher: return 1;
    default: return 0;
    }
}

Cell cardinal_toward(Cell from, Cell to, Cell fallback) {
    const Cell difference = to - from;
    if (std::abs(difference.x) >= std::abs(difference.y) && difference.x != 0)
        return {difference.x > 0 ? 1 : -1, 0};
    if (difference.y != 0) return {0, difference.y > 0 ? 1 : -1};
    return fallback;
}

void blast(Game& game, Cell center, int radius, int damage, Cell attacker) {
    for (int y = center.y - radius; y <= center.y + radius; ++y) {
        for (int x = center.x - radius; x <= center.x + radius; ++x) {
            const Cell cell{x, y};
            if (distance(cell, center) > radius) continue;
            damage_tile(game.stage, cell, damage);
            for (int slot = 0; slot < max_entities; ++slot) {
                Entity& target = game.entities[static_cast<std::size_t>(slot)];
                if (target.kind != EntityKind::None && target.cell == cell &&
                    target.kind != EntityKind::GroundItem && target.kind != EntityKind::RailLayer)
                    damage_entity(game, slot, damage, attacker);
            }
        }
    }
}

bool fire_weapon(Game& game, int user_slot, Cell direction, Item& item) {
    if (item.loaded <= 0) return false;
    Entity& user = game.entities[static_cast<std::size_t>(user_slot)];
    const int range = item.kind == ItemKind::Pistol ? 9 : 14;
    const int damage = item.kind == ItemKind::Pistol ? 16 :
                       (item.kind == ItemKind::RocketLauncher ? 80 : 35);
    Cell cell = user.cell;
    for (int step = 0; step < range; ++step) {
        cell = cell + direction;
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr) break;
        if (!walkable(tile->kind)) {
            if (item.kind == ItemKind::RocketLauncher) blast(game, cell, 2, damage, user.cell);
            else if (item.kind == ItemKind::Musket) damage_tile(game.stage, cell, 25);
            break;
        }
        const int target = entity_at(game, cell, true);
        if (target >= 0 && target != user_slot) {
            if (item.kind == ItemKind::RocketLauncher) blast(game, cell, 2, damage, user.cell);
            else damage_entity(game, target, damage, user.cell);
            break;
        }
        if (step == range - 1 && item.kind == ItemKind::RocketLauncher)
            blast(game, cell, 2, damage, user.cell);
    }
    --item.loaded;
    item.cooldown = item.kind == ItemKind::Pistol ? 12 : 40;
    return true;
}

bool shove(Game& game, int user_slot, Cell direction) {
    const Cell front = game.entities[static_cast<std::size_t>(user_slot)].cell + direction;
    const int target_slot = entity_at(game, front, true);
    if (target_slot < 0 || target_slot == user_slot) return false;
    Entity& target = game.entities[static_cast<std::size_t>(target_slot)];
    if (target.kind == EntityKind::Train) return false;
    const Cell destination = front + direction;
    const Tile* tile = game.stage.at(destination);
    const int blocker_slot = entity_at(game, destination, true);
    const bool hard_tile = tile == nullptr || !walkable(tile->kind);
    const bool hard_actor = blocker_slot >= 0 &&
        game.entities[static_cast<std::size_t>(blocker_slot)].hard_blocker;
    if (hard_tile || hard_actor) {
        target.health = 0;
        return true;
    }
    // Ordinary teammates and loose items do not turn a shove into a crush.
    if (blocker_slot >= 0) return false;
    target.cell = destination;
    target.move_wait = target.move_interval;
    return true;
}

} // namespace

void damage_entity(Game& game, int slot, int damage, Cell attacker) {
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    if (entity.kind == EntityKind::None || entity.kind == EntityKind::GroundItem ||
        entity.kind == EntityKind::RailLayer || entity.kind == EntityKind::Key ||
        entity.kind == EntityKind::Door || entity.kind == EntityKind::Exit || damage <= 0) return;
    Item* held = entity.inventory.held();
    if (entity.block_ticks > 0 && held->kind == ItemKind::Buckler &&
        entity.facing == cardinal_toward(entity.cell, attacker, entity.facing)) {
        held->durability -= std::max(1, damage);
        if (held->durability <= 0) *held = {};
        return;
    }
    entity.health = std::max(0, entity.health - damage);
    entity.use_flash = 6;
    if (entity.health == 0 && entity.kind == EntityKind::Player) {
        entity.impassable = false;
        entity.sprite = Sprite::PlayerDead;
        entity.spawn_wait = 180;
    }
    if (entity.health == 0 && entity.kind == EntityKind::Zombie) {
        for (std::size_t owner = 0; owner < game.players.size(); ++owner) {
            const Entity* player = get_entity(game, game.players[owner]);
            if (player != nullptr && player->cell == attacker) {
                game.run.coins[owner] += 5;
                break;
            }
        }
    }
}

bool use_held_item(Game& game, int user_slot, Cell target) {
    Entity& user = game.entities[static_cast<std::size_t>(user_slot)];
    Item& item = *user.inventory.held();
    if (item.kind == ItemKind::None || item.count <= 0 || item.cooldown > 0) return false;
    const Cell direction = cardinal_toward(user.cell, target, user.facing);
    user.facing = direction;
    const int range = distance(user.cell, target);
    bool used = false;
    int cooldown = 0;
    bool consumed = false;
    switch (item.kind) {
    case ItemKind::Wall:
        if (range >= 1 && range <= 2) {
            Tile* tile = game.stage.at(target);
            if (tile != nullptr && buildable(tile->kind) && entity_at(game, target, true) < 0) {
                *tile = {TileKind::Wall, 100, 0};
                used = consumed = true;
                cooldown = 6;
            }
        }
        break;
    case ItemKind::Medkit: case ItemKind::Bandage: case ItemKind::Bandaid:
        if (user.health < user.max_health) {
            const int heal = item.kind == ItemKind::Medkit ? 100 :
                             (item.kind == ItemKind::Bandage ? 10 : 1);
            user.health = std::min(user.max_health, user.health + heal);
            used = consumed = true;
            cooldown = item.kind == ItemKind::Medkit ? 300 :
                       (item.kind == ItemKind::Bandage ? 120 : 12);
        }
        break;
    case ItemKind::Fist:
        if (range == 1) {
            const int hit = entity_at(game, target, true);
            if (hit >= 0 && hit != user_slot) {
                damage_entity(game, hit, 10, user.cell);
                used = true;
            } else used = damage_tile(game.stage, target, 10);
            cooldown = 12;
        }
        break;
    case ItemKind::ConductorHat: {
        const Handle rail = spawn_entity(game, EntityKind::RailLayer,
                                         {game.stage.width, user.cell.y});
        used = consumed = get_entity(game, rail) != nullptr;
        break;
    }
    case ItemKind::Buckler:
        user.block_ticks = 15;
        shove(game, user_slot, direction);
        used = true;
        cooldown = 30;
        break;
    case ItemKind::Pistol: case ItemKind::Musket: case ItemKind::Bow:
    case ItemKind::RocketLauncher:
        used = fire_weapon(game, user_slot, direction, item);
        return used;
    case ItemKind::Bomb:
        if (range <= 3) {
            blast(game, target, 2, 65, user.cell);
            used = consumed = true;
            cooldown = 45;
        }
        break;
    case ItemKind::Ammo:
        for (Item& weapon : user.inventory.slots) {
            if (is_gun(weapon.kind)) {
                weapon.spare += weapon.kind == ItemKind::RocketLauncher ? 2 : 12;
                used = true;
            }
        }
        consumed = used;
        break;
    case ItemKind::None:
        break;
    }
    if (used) {
        item.cooldown = cooldown;
        user.use_flash = 8;
        if (consumed && --item.count <= 0) item = {};
    }
    return used;
}

bool reload_held_item(Game& game, int user_slot) {
    Item& item = *game.entities[static_cast<std::size_t>(user_slot)].inventory.held();
    const int capacity = magazine_size(item.kind);
    if (capacity == 0 || item.loaded >= capacity || item.spare <= 0 || item.cooldown > 0)
        return false;
    const int transfer = std::min(capacity - item.loaded, item.spare);
    item.loaded += transfer;
    item.spare -= transfer;
    item.cooldown = item.kind == ItemKind::Pistol ? 45 : 60;
    return true;
}
