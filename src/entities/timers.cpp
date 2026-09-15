#include "../items/emergency_foam.hpp"
#include "audit_clerk.hpp"
#include "walking_kiln.hpp"
#include "../items/pocket_drill.hpp"
#include "cable_crawler.hpp"
#include "pressure_rat.hpp"
#include "magnet_crane.hpp"
#include "arc_welder.hpp"
#include "yeti.hpp"
#include "../items/sled.hpp"
#include "../items/ice_anchor.hpp"
#include "../items/effigy_mask.hpp"
#include "../items/thaw_charge.hpp"
#include "../items/echo_pebble.hpp"
#include "../items/flare.hpp"
#include "../items/storm_lantern.hpp"
#include "dispatch.hpp"
#include "../items/quarry_charge.hpp"
#include "../projectiles/exposed_fuse.hpp"
#include "snow_effigy.hpp"
#include "candle_keeper.hpp"
#include "shard_colony.hpp"
#include "boiler_porter.hpp"
#include "boiler_tank.hpp"
#include "icicle_spider.hpp"
#include "../items/fire.hpp"
#include "../world/water.hpp"
#include "../surfaces/interaction.hpp"
#include "../surfaces/temperature.hpp"
#include "../world/encounter.hpp"

#include <algorithm>
#include <cstdlib>
#include <optional>

namespace {

std::optional<Cell> free_entrance_cell(const Game& game) {
    if (const auto inside = encounter_join_cell(game)) return inside;
    for (int radius = 0; radius <= 8; ++radius) {
        for (int dy = -radius; dy <= radius; ++dy) {
            const int dx = radius - std::abs(dy);
            for (int side = -1; side <= 1; side += 2) {
                const Cell cell = game.run.spawn + Cell{dx * side, dy};
                const Tile* tile = game.stage.at(cell);
                if (tile != nullptr && walkable(*tile) &&
                    entity_at(game, cell, true) < 0) return cell;
            }
        }
    }
    return std::nullopt;
}

} // namespace

void step_entity_timers(Game& game, int slot) {
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    if (entity.kind == EntityKind::None) return;
    if (game.tick % static_cast<std::uint64_t>(movement_slow_factor(entity)) == 0)
        entity.move_wait = std::max(0, entity.move_wait - movement_recovery_rate(entity));
    wet_landed_fuse(game,slot);
    if (!exposed_fuse(entity) || entity.freeze_ticks==0 || game.tick%2!=0)
        entity.timer_a = std::max(0, entity.timer_a - 1);
    entity.timer_b = std::max(0, entity.timer_b - 1);
    entity.timer_c = std::max(0, entity.timer_c - 1);
    if (entity.timer_c == 0) { entity.label_c = 0; entity.point_c = {}; }
    entity.attack_wait = std::max(0, entity.attack_wait - 1);
    entity.block_ticks = entity.guard_slot == entity.inventory.selected ?
        std::max(0, entity.block_ticks - 1) : 0;
    entity.use_flash = std::max(0, entity.use_flash - 1);
    entity.fire_dim_ticks = std::max(0, entity.fire_dim_ticks - 1);

    // HEAT FEEDER: Flames feed leeches; physical hits and scalds still hurt them.
    if (entity.kind == EntityKind::SteamLeech || entity.kind==EntityKind::WalkingKiln) entity.scorch_ticks = entity.burn_ticks = 0;
    step_foam_can(game,slot);
    step_thaw_charge(game,slot);
    step_quarry_charge(game,slot);
    step_flare(game,slot);
    step_echo_pebble(game,slot);
    if (entity.kind == EntityKind::None) return;
    step_vital_effects(game, slot);
    contact_surface(game, slot);
    sled_contact(game,slot);
    step_keeper_lamp(game, slot);
    if (entity.kind == EntityKind::BoilerTank && entity.health > 0) step_boiler_tank(game,slot);
    if (entity.kind == EntityKind::BoilerPorter && (entity.sleep_ticks > 0 || entity.stun_ticks > 0))
        interrupt_boiler_porter(entity);
    if (entity.sleep_ticks>0 || entity.stun_ticks>0 || entity.toss.ticks>0) {
        interrupt_audit_clerk(entity);interrupt_walking_kiln(entity);interrupt_yeti(entity); interrupt_arc_welder(entity); interrupt_magnet_crane(entity);
        cool_pressure_rat(entity);interrupt_cable_crawler(entity);interrupt_pocket_drill(entity);
    }
    step_shard_state(entity);
    step_spider_strand(game,entity);
    thaw_snow_effigy(game, slot);

    // HAZARDS: Damage resolves before this tick's action, even on a fatal hit.
    if (entity.scorch_ticks > 0 && entity.health > 0) {
        --entity.scorch_ticks;
        if (game.tick % 30 == 0)
            damage_entity(game, slot, 2, entity.cell, false);
        if (entity.health > 0 && entity.scorch_ticks > 0 && game.tick % 90 == 0)
            emit_sound(game, entity.kind==EntityKind::Sled ? SoundId::SledBurn : SoundId::FirePanic, entity.cell);
    }
    if (entity.burn_ticks > 0) {
        --entity.burn_ticks;
        if (game.tick % 30 == 0)
            damage_entity(game, slot, 4, entity.cell, false);
    }
    entity.freeze_ticks = entity.freeze_ticks > 0 && warm_cell(game, entity.cell) ?
        0 : std::max(0, entity.freeze_ticks - 1);
    entity.sleep_ticks = std::max(0, entity.sleep_ticks - 1);
    entity.stun_ticks = std::max(0, entity.stun_ticks - 1);
    const Tile* ground = game.stage.at(entity.cell);
    if (entity.toss.ticks==0 && ground != nullptr && ground->kind == TileKind::Lava &&
        entity.kind != EntityKind::Ember && entity.kind != EntityKind::SteamLeech && game.tick % 30 == 0)
        damage_entity(game, slot, 5, entity.cell, false);

    // RESPAWN: A blocked entrance delays return instead of overlapping a fixture.
    if (entity.kind == EntityKind::Player && entity.health == 0 &&
        game.run.death_policy == DeathPolicy::Entrance &&
        game.run.phase == RunPhase::Playing) {
        entity.spawn_wait = std::max(0, entity.spawn_wait - 1);
        if (entity.spawn_wait == 0) {
            if (const auto cell = free_entrance_cell(game)) {
                entity.cell = *cell;
                entity.health = entity.max_health;
                entity.impassable = entity.owner < 0 || entity.owner >= 4 ||
                    game.run.online[static_cast<std::size_t>(entity.owner)];
                entity.sprite = Sprite::Player;
                entity.scorch_ticks = entity.burn_ticks = 0;
                entity.sleep_ticks = entity.stun_ticks = entity.freeze_ticks = 0;
                entity.vitals = {};
            } else entity.spawn_wait = 1;
        }
    }
    const bool wet = ground != nullptr && surface_wet(*ground);
    // PAYLOAD: A melee windup copy is not another burning object in the world.
    step_item_state(game, entity.ground_item, entity.cell, wet && entity.kind == EntityKind::GroundItem);
    stow_effigy_mask(entity.ground_item,false);
    if (entity.kind==EntityKind::GroundItem && entity.ground_item.kind==ItemKind::IceAnchor) {
        sync_ice_anchor(game,entity.ground_item);
        if (entity.ground_item.kind==ItemKind::None) { remove_entity(game,{slot,entity.generation}); return; }
    }
    step_lantern_fuel(game,entity.ground_item,entity.cell,entity.kind==EntityKind::GroundItem,false);
    for (int index=0;index<quick_slots;++index) {
        Item& item=entity.inventory.slots[static_cast<std::size_t>(index)];
        step_item_state(game,item,entity.cell,wet && wading_actor(entity));
        sync_ice_anchor(game,item);
        stow_effigy_mask(item,entity.health>0 && entity.sleep_ticks==0 && entity.stun_ticks==0 && index==entity.inventory.selected);
        step_lantern_fuel(game,item,entity.cell,entity.health>0 && index==entity.inventory.selected,true);
    }
    if (entity.kind == EntityKind::GroundItem && entity.ground_item.flame_ticks > 0)
        ignite_surface(game, entity.cell);
}
