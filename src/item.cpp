#include "game.hpp"
#include "projectiles/projectile.hpp"
#include "items/catalog.hpp"
#include "items/woodland_tools.hpp"
#include "items/materials.hpp"
#include "items/firearms.hpp"
#include "item_attribute.hpp"
#include "combat/shove.hpp"
#include "entities/dispatch.hpp"
#include "entities/behavior.hpp"
#include "item_pattern.hpp"
#include "world/ground_items.hpp"
#include "props/interaction.hpp"

#include <algorithm>
#include <cstdlib>

namespace {

int magazine_size(ItemKind kind) {
    if (const RegionalItem* spec = regional_item(kind)) return spec->magazine;
    switch (kind) {
    case ItemKind::Pistol: return 12;
    case ItemKind::Shotgun: return 6;
    case ItemKind::SMG: return 30;
    case ItemKind::Musket: case ItemKind::RocketLauncher: return 1;
    default: return 0;
    }
}


void blast(Game& game, Cell center, int radius, int damage, Cell attacker) {
    emit_sound(game, SoundId::Explosion, center);
    for (int y = center.y - radius; y <= center.y + radius; ++y) {
        for (int x = center.x - radius; x <= center.x + radius; ++x) {
            const Cell cell{x, y};
            if (distance(cell, center) > radius) continue;
            hit_prop(game, cell, damage, attacker);
            hit_terrain(game, cell, attacker, damage, 2, TileImpact::Blast);
            for (int slot = 0; slot < max_entities; ++slot) {
                Entity& target = game.entities[static_cast<std::size_t>(slot)];
                if (target.kind != EntityKind::None && target.cell == cell &&
                    target.kind != EntityKind::GroundItem && target.kind != EntityKind::RailLayer)
                    damage_entity(game, slot, damage, attacker);
            }
        }
    }
}


} // namespace

void blast_area(Game& game, Cell center, int radius, int damage, Cell attacker) {
    blast(game, center, radius, damage, attacker);
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
    case ItemKind::ResinGlue: case ItemKind::SeedBag: case ItemKind::LanternSeed:
        used = use_woodland_tool(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::Torch: case ItemKind::Lighter: case ItemKind::OilFlask:
    case ItemKind::SapJar: case ItemKind::WaterFlask: case ItemKind::MushroomSpores:
    case ItemKind::SmokePot: case ItemKind::HoneyPot:
        used = use_material_item(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::Wall:
        if (range >= 1 && range <= 2) {
            Tile* tile = game.stage.at(target);
            if (tile != nullptr && buildable(tile->kind) && !prop_blocks(tile->prop) &&
                entity_at(game, target, true) < 0) {
                *tile = {TileKind::Wall, 100, 0, 100, BreakRule::Damageable, 0};
                used = true;
                cooldown = 6;
            }
        }
        break;
    case ItemKind::Medkit: case ItemKind::Bandage: case ItemKind::Bandaid:
    case ItemKind::RawMeat: case ItemKind::CookedMeat:
    case ItemKind::Egg: case ItemKind::FriedEgg:
        if (user.health < user.max_health) {
            const ItemPattern pattern = item_pattern(item);
            user.health = std::min(user.max_health, user.health + pattern.heal);
            used = true;
            cooldown = pattern.cooldown;
        }
        break;
    case ItemKind::Hatchet: case ItemKind::HuntingSpear: case ItemKind::WoodenMaul:
    case ItemKind::DiggingClaws: case ItemKind::Rake: case ItemKind::FlintKnife:
    case ItemKind::Fist: case ItemKind::Stick: case ItemKind::Pickaxe:
        if (range >= 1 && range <= item_pattern(item).maximum) {
            const ItemPattern pattern = item_pattern(item);
            strike_melee(game, user_slot, direction, item);
            used = true;
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
        shove_in_front(game, user_slot, direction);
        used = true;
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::Crossbow: case ItemKind::Blunderbuss:
    case ItemKind::Pistol: case ItemKind::Musket:
    case ItemKind::RocketLauncher: case ItemKind::Shotgun: case ItemKind::SMG:
        used = fire_weapon(game, user_slot, direction, item);
        return used;
    case ItemKind::Bow: return false; // Draw/release is handled by the player action step.
    case ItemKind::ThrowingRock:
        used = throw_rock(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::Bomb:
        if (range <= 3) {
            const ItemPattern pattern = item_pattern(item);
            used = launch_projectile(game, user_slot, item, direction, pattern.maximum);
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
            if (tile != nullptr && walkable(*tile) &&
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
            if (item_is_gun(weapon.kind)) {
                if (weapon.kind == ItemKind::Bow) weapon.loaded += 24;
                else weapon.spare += weapon.kind == ItemKind::RocketLauncher ? 2 :
                                std::max(12, magazine_size(weapon.kind) * 3);
                used = true;
            }
        }
        break;
    case ItemKind::Count: case ItemKind::None:
        break;
    }
    if (used) {
        item.cooldown = cooldown;
        user.use_flash = 8;
        if (const RegionalItem* spec = regional_item(used_kind)) {
            if (!item_is_melee(used_kind)) emit_sound(game, spec->sound, user.cell);
        }
        else switch (used_kind) {
        case ItemKind::Wall: emit_sound(game, SoundId::BlockLand, target); break;
        case ItemKind::Medkit: case ItemKind::Bandage: case ItemKind::Bandaid:
            emit_sound(game, SoundId::ClothRip, user.cell); break;
        case ItemKind::RawMeat: case ItemKind::CookedMeat:
            emit_sound(game, SoundId::MeatMunch, user.cell); break;

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
    const RegionalItem* spec = regional_item(item.kind);
    item.cooldown = spec != nullptr ? spec->reload : item.kind == ItemKind::Pistol ? 45 : 60;
    emit_sound(game, firearm_reload_sound(item.kind), game.entities[static_cast<std::size_t>(user_slot)].cell);
    return true;
}
