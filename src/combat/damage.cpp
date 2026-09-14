#include "../game.hpp"
#include "../entities/ice_mason.hpp"
#include "shove.hpp"
#include "../entities/attacks.hpp"
#include "../entities/dispatch.hpp"
#include "../entities/behavior.hpp"
#include "../item_pattern.hpp"
#include "../world/loot.hpp"

#include <algorithm>

namespace {

void apply_health_damage(Game& game, int slot, int damage, Cell attacker) {
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    if (entity.health <= 0 || damage <= 0 || entity.kind == EntityKind::Encounter ||
        entity.kind == EntityKind::EncounterGate || entity.kind == EntityKind::WaveVent ||
        entity.kind == EntityKind::Coins || entity.kind == EntityKind::PocketDoor) return;
    remember_attacker(game, slot, attacker);
    entity.health = std::max(0, entity.health - damage);
    interrupt_ice_mason(entity);
    if (interrupt_recovery(entity)) emit_sound(game, SoundId::BrothSpill, entity.cell);
    entity.use_flash = 6;
    entity.sleep_ticks = 0;
    if (entity.kind == EntityKind::CrateMimic) entity.counter_a = 0;
    if (entity.health == 0 && entity.kind == EntityKind::Trap) return;
    if (entity.health == 0) emit_sound(game, entity.kind == EntityKind::IceMason ? SoundId::MasonDeath : entity.kind == EntityKind::SteamLeech ? SoundId::LeechDeath : entity.kind == EntityKind::BellDiver ? SoundId::DiverDeath : entity.kind == EntityKind::FrostBat ? SoundId::FrostDeath : entity.kind == EntityKind::RimeSkater ? SoundId::SkaterBreak : entity.kind == EntityKind::WaspNest ? SoundId::NestBreak : entity.kind == EntityKind::CrateMimic ?
        SoundId::WoodCrack : entity.kind == EntityKind::RootTurret || entity.kind == EntityKind::BrambleGuard ?
        SoundId::WoodCrack : entity.kind == EntityKind::ThornSnail ?
        SoundId::ShellKnock : SoundId::AnimalCrush1, entity.cell);
    if (entity.health == 0) { entity.vitals = {}; drop_enemy_loot(game, entity); }
    if (entity.health == 0 && entity.kind == EntityKind::Player) {
        entity.impassable = false;
        entity.sprite = Sprite::PlayerDead;
        entity.spawn_wait = 180;
    }
    if (entity.health == 0) topple_zombie_stack(game, slot);
}

} // namespace

void crush_entity(Game& game, int slot, Cell attacker) {
    apply_health_damage(game, slot, 1000000, attacker);
}

void damage_entity(Game& game, int slot, int damage, Cell attacker, bool blockable) {
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    if (entity.kind == EntityKind::Projectile || entity.kind == EntityKind::None || entity.kind == EntityKind::GroundItem ||
        entity.kind == EntityKind::RailLayer || entity.kind == EntityKind::Key ||
        entity.kind == EntityKind::Door || entity.kind == EntityKind::Exit ||
        entity.kind == EntityKind::Switch || entity.kind == EntityKind::Campfire || entity.kind == EntityKind::PocketDoor ||
        entity.kind == EntityKind::Crusher ||
        damage <= 0) return;
    Item* held = entity.inventory.held();
    if (blockable && blocks_facing(entity, attacker)) {
        held->durability -= std::max(1, damage);
        const bool lantern = held->kind == ItemKind::ShieldLantern;
        emit_sound(game, lantern ? SoundId::LanternBlock : SoundId::SturdyBlockBouncedOn, entity.cell);
        if (held->durability <= 0) {
            if (lantern) emit_sound(game, SoundId::LanternBreak, entity.cell);
            *held = {};
            entity.block_ticks = 0;
        }
        return;
    }
    damage = enemy_defense(game, slot, damage, attacker, blockable);
    apply_health_damage(game, slot, damage, attacker);
    if (blockable && entity.health > 0 && has_artifact(entity, ArtifactKind::Reflector) &&
        random_u32(game) % 4 == 0) {
        const int reflected = entity_at(game, attacker, true);
        if (reflected >= 0 && reflected != slot)
            apply_health_damage(game, reflected, std::max(1, damage / 2), entity.cell);
    }
}

