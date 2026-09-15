#include "../items/action.hpp"
#include "../entities/mine_crew.hpp"
#include "../game.hpp"
#include "../entities/ember.hpp"
#include "../entities/powder_monkey.hpp"
#include "../entities/icicle_spider.hpp"
#include "../entities/boiler_porter.hpp"
#include "../entities/boiler_tank.hpp"
#include "../entities/ice_mason.hpp"
#include "../entities/glass_eel.hpp"
#include "../entities/snow_burrower.hpp"
#include "../entities/mirror_knight.hpp"
#include "../entities/lens_warden.hpp"
#include "../entities/echo_hound.hpp"
#include "../entities/frozen_pilgrim.hpp"
#include "../entities/fishing_widow.hpp"
#include "../entities/seal_thief.hpp"
#include "../entities/whiteout_drummer.hpp"
#include "../entities/avalanche_ram.hpp"
#include "../entities/snow_effigy.hpp"
#include "../entities/candle_keeper.hpp"
#include "../entities/shard_colony.hpp"
#include "../entities/death_sound.hpp"
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
    // The overhead press swing is deliberately vulnerable; a blocked hit never
    // reaches this path. An interrupted windup spends no tool condition.
    if (entity.kind == EntityKind::Player && entity.label_b < 0 &&
        entity.ground_item.kind == ItemKind::PressHammer) cancel_item_action(entity);
    hurt_mine_worker(game,slot,damage,attacker);
    interrupt_stoker(entity);
    interrupt_powder_monkey(entity);
    interrupt_whiteout_drummer(entity);
    interrupt_avalanche_ram(entity);
    interrupt_snow_effigy(entity);
    interrupt_candle_keeper(entity);
    interrupt_shard_node(entity);
    interrupt_icicle_spider(game,entity);
    interrupt_boiler_porter(entity);
    damage_boiler(game,slot);
    interrupt_seal_thief(entity);
    interrupt_fishing_widow(entity);
    interrupt_frozen_pilgrim(entity);
    interrupt_echo_hound(entity);
    interrupt_lens_warden(entity);
    interrupt_mirror_knight(entity);
    expose_snow_burrower(entity);
    interrupt_ice_mason(entity);
    interrupt_glass_eel(entity);
    if (interrupt_recovery(entity)) emit_sound(game, SoundId::BrothSpill, entity.cell);
    entity.use_flash = 6;
    entity.sleep_ticks = 0;
    if (entity.kind == EntityKind::CrateMimic) entity.counter_a = 0;
    if (entity.health == 0 && entity.kind == EntityKind::Trap) return;
    if (entity.health == 0) emit_sound(game, entity_death_sound(entity.kind), entity.cell);
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

