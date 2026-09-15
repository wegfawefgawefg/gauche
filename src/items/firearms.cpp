#include "../entities/hearing.hpp"
#include "firearms.hpp"
#include "../projectiles/harpoon.hpp"
#include "muffling.hpp"
#include "../combat/beams.hpp"
#include "catalog.hpp"
#include "../projectiles/projectile.hpp"
#include "../combat/ranged.hpp"

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

} // namespace

SoundId firearm_reload_sound(ItemKind kind) {
    switch (kind) {
    case ItemKind::RivetGun: return SoundId::RivetReload;
    case ItemKind::HarpoonGun: return SoundId::HarpoonReload;
    case ItemKind::LensCarbine: return SoundId::LensReload;
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
    if (item.kind == ItemKind::HarpoonGun) return launch_harpoon(game,user_slot,item,direction);
    const ItemPattern pattern = item_pattern(item);
    if (item.kind == ItemKind::RivetGun || item.kind == ItemKind::Crossbow || item.kind == ItemKind::RocketLauncher) {
        // FLIGHT: Allocation must succeed before spending ammunition or recovery.
        if (!launch_projectile(game, user_slot, item, direction, pattern.maximum)) return false;
        if (item.kind==ItemKind::RivetGun && item.muffled_uses==0) make_noise(game,user.cell,8);
        finish_muffled_use(game, item, user.cell);
        --item.loaded;
        item.cooldown = pattern.cooldown;
        user.use_flash = 6;
        return true;
    }
    const Cell origin = user.cell, sideways{-direction.y, direction.x};
    for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane) {
        const Cell source = origin + Cell{sideways.x * lane, sideways.y * lane};
        if (item.kind == ItemKind::LensCarbine)
            resolve_beam(game, trace_beam(game, source, direction, pattern.damage, pattern.maximum,
                pattern.piercing || has_artifact(user, ArtifactKind::AllPiercing)));
        else fire_bullet(game, user_slot, source, direction, item, lane == 0);
    }
    --item.loaded;
    item.cooldown = pattern.cooldown;
    user.use_flash = 6;
    emit_weapon_sound(game, item, user.kind == EntityKind::Ember ? SoundId::SmallLaser : firing_sound(item.kind), origin);
    finish_muffled_use(game, item, origin);
    if (item.kind == ItemKind::Blunderbuss) {
        const Cell facing = user.facing;
        move_entity(game, user_slot, origin - direction);
        user.facing = facing;
    }
    return true;
}
