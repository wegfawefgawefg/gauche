#include "game.hpp"
#include "item_pattern.hpp"

#include <algorithm>
#include <cstdlib>

namespace {

bool is_gun(ItemKind kind) {
    return kind == ItemKind::Pistol || kind == ItemKind::Musket ||
           kind == ItemKind::Bow || kind == ItemKind::RocketLauncher ||
           kind == ItemKind::Shotgun || kind == ItemKind::SMG;
}

int magazine_size(ItemKind kind) {
    switch (kind) {
    case ItemKind::Pistol: return 12;
    case ItemKind::Shotgun: return 6;
    case ItemKind::SMG: return 30;
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
    emit_sound(game, SoundId::Explosion, center);
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
    const ItemPattern pattern = item_pattern(item);
    const bool piercing = pattern.piercing ||
        (has_artifact(user, ArtifactKind::AllPiercing) &&
         item.kind != ItemKind::RocketLauncher);
    const int range = pattern.maximum;
    const int damage = pattern.damage;
    const int cooldown = pattern.cooldown;
    Cell cell = user.cell;
    for (int step = 0; step < range; ++step) {
        cell = cell + direction;
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr) break;
        if (!walkable(tile->kind)) {
            if (item.kind == ItemKind::RocketLauncher)
                blast(game, cell, pattern.blast_radius, damage, user.cell);
            else if (item.kind == ItemKind::Musket) damage_tile(game.stage, cell, 25);
            break;
        }
        const int target = entity_at(game, cell, true);
        if (target >= 0 && target != user_slot) {
            if (item.kind == ItemKind::RocketLauncher)
                blast(game, cell, pattern.blast_radius, damage, user.cell);
            else {
                const int prior_health = game.entities[static_cast<std::size_t>(target)].health;
                damage_entity(game, target, damage, user.cell);
                if (user.kind == EntityKind::Ember &&
                    game.entities[static_cast<std::size_t>(target)].health < prior_health)
                    game.entities[static_cast<std::size_t>(target)].burn_ticks = 120;
            }
            if (!piercing) break;
        }
        if (step == range - 1 && item.kind == ItemKind::RocketLauncher)
            blast(game, cell, pattern.blast_radius, damage, user.cell);
    }
    --item.loaded;
    item.cooldown = cooldown;
    emit_sound(game, item.kind == ItemKind::RocketLauncher ? SoundId::Explosion1 :
               SoundId::SmallLaser, user.cell);
    return true;
}

bool shove(Game& game, int user_slot, Cell direction) {
    const Cell front = game.entities[static_cast<std::size_t>(user_slot)].cell + direction;
    int target_slot = entity_at(game, front, true);
    if (target_slot < 0) {
        for (int slot = 0; slot < max_entities; ++slot) {
            const Entity& candidate = game.entities[static_cast<std::size_t>(slot)];
            if (candidate.kind == EntityKind::GroundItem && candidate.cell == front) {
                target_slot = slot;
                break;
            }
        }
    }
    if (target_slot < 0 || target_slot == user_slot) return false;
    Entity& target = game.entities[static_cast<std::size_t>(target_slot)];
    if (target.hard_blocker) return false;
    const Cell destination = front + direction;
    const Tile* tile = game.stage.at(destination);
    const int blocker_slot = entity_at(game, destination, true);
    const bool hard_tile = tile == nullptr || !walkable(tile->kind);
    const bool hard_actor = blocker_slot >= 0 &&
        game.entities[static_cast<std::size_t>(blocker_slot)].hard_blocker;
    if (hard_tile || hard_actor) {
        if (target.kind == EntityKind::GroundItem)
            remove_entity(game, {target_slot, target.generation});
        else crush_entity(game, target_slot, game.entities[static_cast<std::size_t>(user_slot)].cell);
        return true;
    }
    // Ordinary teammates and loose items do not turn a shove into a crush.
    if (blocker_slot >= 0) return false;
    if (target.kind == EntityKind::GroundItem) {
        for (const Entity& other : game.entities)
            if (&other != &target && other.kind == EntityKind::GroundItem &&
                other.cell == destination) return false;
    }
    target.cell = destination;
    target.move_wait = target.move_interval;
    return true;
}

bool strike_melee(Game& game, int user_slot, Cell direction,
                  ItemKind kind, ItemPattern pattern) {
    const Cell origin = game.entities[static_cast<std::size_t>(user_slot)].cell;
    const Cell sideways{-direction.y, direction.x};
    bool struck = false;
    for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane) {
        for (int reach = 1; reach <= pattern.maximum; ++reach) {
            const Cell cell = origin + Cell{direction.x * reach + sideways.x * lane,
                                            direction.y * reach + sideways.y * lane};
            const Tile* tile = game.stage.at(cell);
            if (tile == nullptr) break;
            const bool blocked = !walkable(tile->kind);
            const int hit = entity_at(game, cell, true);
            if (hit >= 0 && hit != user_slot) {
                damage_entity(game, hit, pattern.damage, origin);
                struck = true;
                break;
            }
            struck |= damage_tile(game.stage, cell,
                                   kind == ItemKind::Pickaxe ? 50 : pattern.damage);
            if (blocked) break;
        }
    }
    return struck;
}

} // namespace

void blast_area(Game& game, Cell center, int radius, int damage, Cell attacker) {
    blast(game, center, radius, damage, attacker);
}

namespace {

void apply_health_damage(Game& game, int slot, int damage, Cell attacker) {
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    if (entity.health <= 0 || damage <= 0) return;
    entity.health = std::max(0, entity.health - damage);
    entity.use_flash = 6;
    entity.sleep_ticks = 0;
    if (entity.health == 0) emit_sound(game, SoundId::AnimalCrush1, entity.cell);
    if (entity.health == 0 &&
        (entity.kind == EntityKind::Chicken || entity.kind == EntityKind::Bunny) &&
        random_u32(game) % 10 == 0) {
        const Handle meat = spawn_entity(game, EntityKind::GroundItem, entity.cell);
        if (Entity* drop = get_entity(game, meat)) {
            drop->ground_item = make_item(ItemKind::RawMeat);
            drop->sprite = Sprite::RawMeat;
        }
    }
    if (entity.health == 0 && entity.kind == EntityKind::Player) {
        entity.impassable = false;
        entity.sprite = Sprite::PlayerDead;
        entity.spawn_wait = 180;
    }
    if (entity.health == 0 && entity.kind != EntityKind::Player &&
        entity.kind != EntityKind::Chicken && entity.kind != EntityKind::Bunny &&
        entity.kind != EntityKind::Train && entity.kind != EntityKind::Trap) {
        for (std::size_t owner = 0; owner < game.players.size(); ++owner) {
            const Entity* player = get_entity(game, game.players[owner]);
            if (player != nullptr && player->cell == attacker) {
                game.run.coins[owner] += 5;
                break;
            }
        }
    }
}

} // namespace

void crush_entity(Game& game, int slot, Cell attacker) {
    apply_health_damage(game, slot, 1000000, attacker);
}

void damage_entity(Game& game, int slot, int damage, Cell attacker) {
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    if (entity.kind == EntityKind::None || entity.kind == EntityKind::GroundItem ||
        entity.kind == EntityKind::RailLayer || entity.kind == EntityKind::Key ||
        entity.kind == EntityKind::Door || entity.kind == EntityKind::Exit ||
        entity.kind == EntityKind::Switch || entity.kind == EntityKind::Campfire ||
        entity.kind == EntityKind::Crusher ||
        damage <= 0) return;
    Item* held = entity.inventory.held();
    if (entity.block_ticks > 0 && held->kind == ItemKind::Buckler &&
        entity.facing == cardinal_toward(entity.cell, attacker, entity.facing)) {
        held->durability -= std::max(1, damage);
        emit_sound(game, SoundId::SturdyBlockBouncedOn, entity.cell);
        if (held->durability <= 0) *held = {};
        return;
    }
    apply_health_damage(game, slot, damage, attacker);
    if (entity.health > 0 && has_artifact(entity, ArtifactKind::Reflector) &&
        random_u32(game) % 4 == 0) {
        const int reflected = entity_at(game, attacker, true);
        if (reflected >= 0 && reflected != slot)
            apply_health_damage(game, reflected, std::max(1, damage / 2), entity.cell);
    }
}

bool use_held_item(Game& game, int user_slot, Cell target) {
    Entity& user = game.entities[static_cast<std::size_t>(user_slot)];
    Item& item = *user.inventory.held();
    if (item.kind == ItemKind::None || item.count <= 0 || item.cooldown > 0) return false;
    const Cell direction = cardinal_toward(user.cell, target, user.facing);
    const ItemKind used_kind = item.kind;
    user.facing = direction;
    const int range = distance(user.cell, target);
    bool used = false;
    int cooldown = 0;
    switch (item.kind) {
    case ItemKind::Wall:
        if (range >= 1 && range <= 2) {
            Tile* tile = game.stage.at(target);
            if (tile != nullptr && buildable(tile->kind) && entity_at(game, target, true) < 0) {
                *tile = {TileKind::Wall, 100, 0};
                used = true;
                cooldown = 6;
            }
        }
        break;
    case ItemKind::Medkit: case ItemKind::Bandage: case ItemKind::Bandaid:
    case ItemKind::RawMeat: case ItemKind::CookedMeat:
        if (user.health < user.max_health) {
            const ItemPattern pattern = item_pattern(item);
            user.health = std::min(user.max_health, user.health + pattern.heal);
            used = true;
            cooldown = pattern.cooldown;
        }
        break;
    case ItemKind::Fist: case ItemKind::Stick: case ItemKind::Pickaxe:
        if (range >= 1 && range <= item_pattern(item).maximum) {
            const ItemPattern pattern = item_pattern(item);
            used = strike_melee(game, user_slot, direction, item.kind, pattern);
            cooldown = pattern.cooldown;
        }
        break;
    case ItemKind::ConductorHat: {
        const Handle rail = spawn_entity(game, EntityKind::RailLayer,
                                         {game.stage.width, user.cell.y});
        used = get_entity(game, rail) != nullptr;
        break;
    }
    case ItemKind::Buckler:
        user.block_ticks = 15;
        shove(game, user_slot, direction);
        used = true;
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::Pistol: case ItemKind::Musket: case ItemKind::Bow:
    case ItemKind::RocketLauncher: case ItemKind::Shotgun: case ItemKind::SMG:
        used = fire_weapon(game, user_slot, direction, item);
        return used;
    case ItemKind::Bomb:
        if (range <= 3) {
            const ItemPattern pattern = item_pattern(item);
            blast(game, target, pattern.blast_radius, pattern.damage, user.cell);
            used = true;
            cooldown = pattern.cooldown;
        }
        break;
    case ItemKind::SleepMeds:
        if (range <= 3) {
            const int victim = entity_at(game, target, true);
            if (victim >= 0 && victim != user_slot) {
                Entity& sleeper = game.entities[static_cast<std::size_t>(victim)];
                sleeper.sleep_ticks = std::max(sleeper.sleep_ticks, 180);
                used = true;
                cooldown = 30;
            }
        }
        break;
    case ItemKind::BearTrap: case ItemKind::Mine:
        if (item.kind == ItemKind::BearTrap && !item.opened) {
            item.opened = true;
            item.cooldown = 10;
            emit_sound(game, SoundId::SturdyBlockBouncedOn, user.cell);
            return true;
        }
        if (range == 1) {
            const Tile* tile = game.stage.at(target);
            if (tile != nullptr && walkable(tile->kind) &&
                entity_at(game, target) < 0) {
                const Handle trap = spawn_entity(game, EntityKind::Trap, target);
                if (Entity* placed = get_entity(game, trap)) {
                    placed->owner = user.owner;
                    placed->ground_item = make_item(item.kind);
                    placed->ground_item.opened = item.kind == ItemKind::BearTrap;
                    placed->sprite = item.kind == ItemKind::BearTrap ?
                        Sprite::BearTrapOpen : item_sprite(item.kind);
                    used = true;
                    cooldown = 20;
                    item.opened = false;
                }
            }
        }
        break;
    case ItemKind::Ammo:
        for (Item& weapon : user.inventory.slots) {
            if (is_gun(weapon.kind)) {
                weapon.spare += weapon.kind == ItemKind::RocketLauncher ? 2 :
                                std::max(12, magazine_size(weapon.kind) * 3);
                used = true;
            }
        }
        break;
    case ItemKind::None:
        break;
    }
    if (used) {
        item.cooldown = cooldown;
        user.use_flash = 8;
        switch (used_kind) {
        case ItemKind::Wall: emit_sound(game, SoundId::BlockLand, target); break;
        case ItemKind::Medkit: case ItemKind::Bandage: case ItemKind::Bandaid:
        case ItemKind::RawMeat: case ItemKind::CookedMeat:
            emit_sound(game, SoundId::ClothRip, user.cell); break;
        case ItemKind::Fist: case ItemKind::Stick: case ItemKind::Pickaxe:
            emit_sound(game, SoundId::Punch1, user.cell); break;
        case ItemKind::ConductorHat:
            emit_sound(game, SoundId::DistantTrainSound, user.cell); break;
        case ItemKind::Buckler: emit_sound(game, SoundId::HitBlock1, user.cell); break;
        case ItemKind::SleepMeds: emit_sound(game, SoundId::ClothRip, target); break;
        case ItemKind::BearTrap: case ItemKind::Mine:
            emit_sound(game, SoundId::BlockLand, target); break;
        default: break;
        }
        if (item.max_uses > 0 && --item.uses <= 0) {
            emit_sound(game, SoundId::BoxBreak, user.cell);
            item = {};
            return true;
        }
        if (item.consume_on_use && --item.count <= 0) item = {};
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
