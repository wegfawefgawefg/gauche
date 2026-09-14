#include "firearms.hpp"
#include "catalog.hpp"
#include "../projectiles/projectile.hpp"
#include "../props/interaction.hpp"

namespace {

SoundId firing_sound(ItemKind kind) {
    switch (kind) {
    case ItemKind::Pistol: return SoundId::PistolShot;
    case ItemKind::Musket: return SoundId::MusketShot;
    case ItemKind::Shotgun: return SoundId::ShotgunShot;
    case ItemKind::SMG: return SoundId::SmgShot;
    default: break;
    }
    const RegionalItem* spec = regional_item(kind);
    return spec != nullptr ? spec->sound : SoundId::SmallLaser;
}

void shot_event(Game& game, Cell source, Cell end, bool impact, bool muzzle, ItemKind kind) {
    if (game.shot_count >= static_cast<int>(game.shots.size())) return;
    const bool casing = kind == ItemKind::Pistol || kind == ItemKind::SMG || kind == ItemKind::Shotgun;
    game.shots[static_cast<std::size_t>(game.shot_count++)] = {source, end, impact, muzzle, casing};
}

} // namespace

SoundId firearm_reload_sound(ItemKind kind) {
    switch (kind) {
    case ItemKind::Pistol: case ItemKind::SMG: return SoundId::PistolReload;
    case ItemKind::Shotgun: return SoundId::ShellReload;
    case ItemKind::Musket: case ItemKind::Blunderbuss: return SoundId::PowderReload;
    case ItemKind::Crossbow: return SoundId::CrossbowReload;
    case ItemKind::RocketLauncher: return SoundId::RocketReload;
    default: return SoundId::WeaponReload;
    }
}

bool fire_weapon(Game& game, int user_slot, Cell direction, Item& item) {
    Entity& user = game.entities[static_cast<std::size_t>(user_slot)];
    if (item.loaded <= 0) {
        item.cooldown = 15;
        emit_sound(game, SoundId::WeaponEmpty, user.cell);
        return false;
    }
    const ItemPattern pattern = item_pattern(item);
    if (item.kind == ItemKind::Crossbow || item.kind == ItemKind::RocketLauncher) {
        // FLIGHT: Allocation must succeed before spending ammunition or recovery.
        if (!launch_projectile(game, user_slot, item, direction, pattern.maximum)) return false;
        --item.loaded;
        item.cooldown = pattern.cooldown;
        user.use_flash = 6;
        return true;
    }
    const bool piercing = pattern.piercing || has_artifact(user, ArtifactKind::AllPiercing);
    const Cell origin = user.cell, sideways{-direction.y, direction.x};
    for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane) {
        const Cell source = origin + Cell{sideways.x * lane, sideways.y * lane};
        Cell cell = source;
        bool impact = false;
        for (int step = 0; step < pattern.maximum; ++step) {
            const Cell next = cell + direction;
            const Tile* tile = game.stage.at(next);
            if (tile == nullptr) break;
            cell = next;
            const bool prop = prop_blocks(tile->prop);
            hit_prop(game, cell, pattern.damage, origin);
            if (prop && !piercing) { impact = true; break; }
            if (tile->kind == TileKind::Wall) {
                hit_terrain(game, cell, origin, pattern.damage, item.dig_power);
                impact = true;
                break;
            }
            const int target = entity_at(game, cell, true);
            if (target < 0 || target == user_slot) continue;
            const int health = game.entities[static_cast<std::size_t>(target)].health;
            damage_entity(game, target, pattern.damage, origin);
            if (user.kind == EntityKind::Ember && game.entities[static_cast<std::size_t>(target)].health < health)
                game.entities[static_cast<std::size_t>(target)].burn_ticks = 120;
            if (!piercing || game.entities[static_cast<std::size_t>(target)].hard_blocker) {
                impact = true;
                break;
            }
        }
        shot_event(game, source, cell, impact, lane == 0,
            user.kind == EntityKind::Ember ? ItemKind::None : item.kind);
    }
    --item.loaded;
    item.cooldown = pattern.cooldown;
    user.use_flash = 6;
    emit_sound(game, user.kind == EntityKind::Ember ? SoundId::SmallLaser : firing_sound(item.kind), origin);
    if (item.kind == ItemKind::Blunderbuss) {
        const Cell facing = user.facing;
        move_entity(game, user_slot, origin - direction);
        user.facing = facing;
    }
    return true;
}
