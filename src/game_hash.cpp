#include "debug/performance.hpp"
#include "game.hpp"

namespace {

void mix(std::uint64_t& hash, std::uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
}

void mix_light(std::uint64_t& hash, LightEmitter light) {
    mix(hash, static_cast<std::uint64_t>(light.radius));
    mix(hash, static_cast<std::uint64_t>(light.strength));
    mix(hash, light.color.red);
    mix(hash, light.color.green);
    mix(hash, light.color.blue);
    mix(hash, static_cast<std::uint64_t>(light.shape));
}

} // namespace

std::uint64_t game_hash(const Game& game) {
    PerfScope perf_scope(PerfZone::Hash);
    std::uint64_t hash = 1469598103934665603ULL;
    mix(hash, game.rng);
    mix(hash, game.tick);
    mix(hash, static_cast<std::uint64_t>(game.started));
    mix(hash, static_cast<std::uint64_t>(game.game_over));
    mix(hash, static_cast<std::uint64_t>(game.run.phase));
    mix(hash, game.run.seed);
    mix(hash, static_cast<std::uint64_t>(game.run.floor));
    mix(hash, static_cast<std::uint64_t>(game.run.layout));
    mix(hash, static_cast<std::uint64_t>(game.run.has_key));
    mix(hash, static_cast<std::uint64_t>(game.run.objective));
    mix(hash, static_cast<std::uint64_t>(game.run.death_policy));
    mix(hash, static_cast<std::uint64_t>(game.run.spawn.x));
    mix(hash, static_cast<std::uint64_t>(game.run.spawn.y));
    mix(hash, static_cast<std::uint64_t>(game.run.exit.x));
    mix(hash, static_cast<std::uint64_t>(game.run.exit.y));
    mix(hash, static_cast<std::uint64_t>(game.run.roof_light_count));
    for (const StageLight& light : game.run.roof_lights) {
        mix(hash, static_cast<std::uint64_t>(light.cell.x));
        mix(hash, static_cast<std::uint64_t>(light.cell.y));
        mix_light(hash, light.light);
    }
    mix(hash, game.players.size());
    for (const auto& [owner, participant] : game.players) {
        mix(hash, static_cast<std::uint64_t>(owner));
        mix(hash, static_cast<std::uint64_t>(participant.controlled.slot));
        mix(hash, participant.controlled.generation);
        mix(hash, static_cast<std::uint64_t>(participant.coins));
        mix(hash, static_cast<std::uint64_t>(participant.chosen));
        mix(hash, static_cast<std::uint64_t>(participant.shop_ready));
        mix(hash, static_cast<std::uint64_t>(participant.online));
        mix(hash, static_cast<std::uint64_t>(participant.pending_count));
        for (const Reward& reward : participant.offers) {
            mix(hash, static_cast<std::uint64_t>(reward.kind));
            mix(hash, static_cast<std::uint64_t>(reward.item));
            mix(hash, static_cast<std::uint64_t>(reward.artifact));
            mix(hash, static_cast<std::uint64_t>(reward.amount));
            mix(hash, static_cast<std::uint64_t>(reward.attribute));
        }
        for (const auto& offer : participant.pending_offers) {
            for (const Reward& reward : offer) {
                mix(hash, static_cast<std::uint64_t>(reward.kind));
                mix(hash, static_cast<std::uint64_t>(reward.item));
                mix(hash, static_cast<std::uint64_t>(reward.artifact));
                mix(hash, static_cast<std::uint64_t>(reward.amount));
                mix(hash, static_cast<std::uint64_t>(reward.attribute));
            }
        }
    }
    for (ItemKind item : game.run.shop_stock) mix(hash, static_cast<std::uint64_t>(item));
    mix(hash, static_cast<std::uint64_t>(game.stage.width));
    mix(hash, static_cast<std::uint64_t>(game.stage.height));
    for (const Tile& tile : game.stage.tiles) {
        mix(hash, static_cast<std::uint64_t>(tile.material));
        mix(hash, static_cast<std::uint64_t>(tile.thaw_kind));
        mix(hash, tile.freeze_ticks);
        mix(hash, static_cast<std::uint64_t>(tile.kind));
        mix(hash, tile.hp);
        mix(hash, tile.max_hp);
        mix(hash, static_cast<std::uint64_t>(tile.break_rule));
        mix(hash, tile.required_dig_power);
        mix(hash, tile.current);
        mix(hash,static_cast<std::uint64_t>(tile.contents));mix(hash,tile.content_count);
        mix(hash, static_cast<std::uint64_t>(tile.surface.liquid));
        mix(hash, tile.surface.gritted);
        mix(hash, tile.surface.reactor_fire);
        for (auto ticks : {tile.surface.liquid_ticks, tile.surface.fire_ticks, tile.surface.smoke_ticks, tile.surface.sleep_ticks, tile.surface.scent_ticks, tile.surface.warmth_ticks, tile.surface.whiteout_ticks, tile.surface.still_ticks}) mix(hash, ticks);
        mix(hash, static_cast<std::uint64_t>(tile.prop.kind));
        mix(hash, tile.prop.hp);
        mix(hash, tile.prop.variant);
        mix(hash, static_cast<std::uint64_t>(tile.prop.broken));
        mix(hash, tile.prop.growth_ticks);
        mix(hash, tile.prop.covered);
    }
    mix(hash,game.stage.prop_owners.size());
    for (const auto& [cell,id] : game.stage.prop_owners) {
        mix(hash,static_cast<std::uint64_t>(cell)); mix(hash,static_cast<std::uint64_t>(id));
    }
    mix(hash,game.stage.roofs.size());
    for (const RoofSpan& roof:game.stage.roofs) {
        mix(hash,static_cast<std::uint64_t>(roof.start.x));mix(hash,static_cast<std::uint64_t>(roof.start.y));
        mix(hash,static_cast<std::uint64_t>(roof.kind));mix(hash,roof.length);mix(hash,roof.vertical);mix(hash,roof.hp);mix(hash,roof.height);mix(hash,roof.width);
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
        mix_light(hash, entity.light);
        mix(hash, entity.self_light.red);
        mix(hash, entity.self_light.green);
        mix(hash, entity.self_light.blue);
        mix(hash, static_cast<std::uint64_t>(entity.owner));
        mix(hash, static_cast<std::uint64_t>(entity.health));
        mix(hash, static_cast<std::uint64_t>(entity.max_health));
        mix(hash, static_cast<std::uint64_t>(entity.move_wait));
        mix(hash, static_cast<std::uint64_t>(entity.move_interval));
        mix(hash, static_cast<std::uint64_t>(entity.attack_wait));
        mix(hash, static_cast<std::uint64_t>(entity.attack_interval));
        mix(hash, static_cast<std::uint64_t>(entity.block_ticks));
        mix(hash, static_cast<std::uint64_t>(entity.guard_slot));
        mix(hash, static_cast<std::uint64_t>(entity.burn_ticks));
        mix(hash, static_cast<std::uint64_t>(entity.scorch_ticks));
        mix(hash, static_cast<std::uint64_t>(entity.fire_dim_ticks));
        mix(hash, entity.fire_tramples);
        mix(hash, static_cast<std::uint64_t>(entity.script_tick));
        mix(hash, static_cast<std::uint64_t>(entity.freeze_ticks));
        mix(hash, static_cast<std::uint64_t>(entity.sleep_ticks));
        mix(hash, static_cast<std::uint64_t>(entity.stun_ticks));
        mix(hash, static_cast<std::uint64_t>(entity.vitals.recovery));
        mix(hash, entity.vitals.chill_guard);
        mix(hash, entity.vitals.summer_ticks);
        mix(hash, entity.vitals.summer_radius);
        for (auto ticks : {entity.vitals.healing_left, entity.vitals.healing_wait,
            entity.vitals.sleep_guard, entity.vitals.stun_guard, entity.vitals.haste, entity.vitals.rooted, entity.vitals.grip, entity.vitals.floor_insulation, entity.vitals.traction, entity.vitals.slide_momentum, entity.vitals.nausea, entity.vitals.nausea_wait, entity.vitals.bleeding, entity.vitals.bleed_wait}) mix(hash, ticks);
        mix(hash, static_cast<std::uint64_t>(entity.vitals.root_kind));
        mix(hash, static_cast<std::uint64_t>(entity.train_cars_left));
        mix(hash, static_cast<std::uint64_t>(entity.train_origin.x));
        mix(hash, static_cast<std::uint64_t>(entity.train_origin.y));
        mix(hash, static_cast<std::uint64_t>(entity.spawn_wait));
        for (Cell cell:{entity.toss.origin,entity.toss.direction,entity.toss.source}) {
            mix(hash,static_cast<std::uint64_t>(cell.x)); mix(hash,static_cast<std::uint64_t>(cell.y));
        }
        mix(hash,static_cast<std::uint64_t>(entity.toss.instigator.slot));
        mix(hash,entity.toss.instigator.generation);
        mix(hash,static_cast<std::uint64_t>(entity.toss.ticks));
        for (Handle handle : {entity.entity_a, entity.entity_b, entity.encounter}) {
            mix(hash, static_cast<std::uint64_t>(handle.slot));
            mix(hash, handle.generation);
        }
        for (Cell cell : {entity.point_a, entity.point_b, entity.point_c}) {
            mix(hash, static_cast<std::uint64_t>(cell.x));
            mix(hash, static_cast<std::uint64_t>(cell.y));
        }
        for (int value : {entity.counter_a, entity.counter_b, entity.counter_c, entity.label_a, entity.label_b, entity.label_c,
                          entity.timer_a, entity.timer_b, entity.timer_c}) mix(hash, static_cast<std::uint64_t>(value));
        mix(hash, static_cast<std::uint64_t>(entity.birth_tick));
        mix(hash, static_cast<std::uint64_t>(entity.impassable));
        mix(hash, static_cast<std::uint64_t>(entity.hard_blocker));
        mix(hash, static_cast<std::uint64_t>(entity.fixture_open));
        mix(hash, entity.artifacts);
        mix(hash, static_cast<std::uint64_t>(entity.inventory.selected));
        for (const Item& item : entity.inventory.slots) {
            mix(hash, static_cast<std::uint64_t>(item.kind));
            mix(hash, static_cast<std::uint64_t>(item.attribute));
            mix(hash, static_cast<std::uint64_t>(item.count));
            mix(hash, static_cast<std::uint64_t>(item.max_count));
            mix(hash, static_cast<std::uint64_t>(item.consume_on_use));
            mix(hash, static_cast<std::uint64_t>(item.cooldown));
            mix(hash, static_cast<std::uint64_t>(item.loaded));
            mix(hash, static_cast<std::uint64_t>(item.spare));
            mix(hash, static_cast<std::uint64_t>(item.durability));
            mix(hash, static_cast<std::uint64_t>(item.max_durability));
            mix(hash, static_cast<std::uint64_t>(item.uses));
            mix(hash, static_cast<std::uint64_t>(item.max_uses));
            mix(hash, static_cast<std::uint64_t>(item.opened));
            mix_light(hash, item.light);
            mix(hash, static_cast<std::uint64_t>(item.dig_power));
            mix(hash, static_cast<std::uint64_t>(item.flame_ticks));
            mix(hash, item.muffled_uses);
            mix(hash, static_cast<std::uint64_t>(item.flight.slot));
            mix(hash, item.flight.generation);
            mix(hash, static_cast<std::uint64_t>(item.anchor.slot));
            mix(hash, item.anchor.generation);
        }
        const Item& ground = entity.ground_item;
        mix(hash, static_cast<std::uint64_t>(ground.kind));
        mix(hash, static_cast<std::uint64_t>(ground.attribute));
        mix(hash, static_cast<std::uint64_t>(ground.count));
        mix(hash, static_cast<std::uint64_t>(ground.max_count));
        mix(hash, static_cast<std::uint64_t>(ground.consume_on_use));
        mix(hash, static_cast<std::uint64_t>(ground.cooldown));
        mix(hash, static_cast<std::uint64_t>(ground.loaded));
        mix(hash, static_cast<std::uint64_t>(ground.spare));
        mix(hash, static_cast<std::uint64_t>(ground.durability));
        mix(hash, static_cast<std::uint64_t>(ground.max_durability));
        mix(hash, static_cast<std::uint64_t>(ground.uses));
        mix(hash, static_cast<std::uint64_t>(ground.max_uses));
        mix(hash, static_cast<std::uint64_t>(ground.opened));
        mix_light(hash, ground.light);
        mix(hash, static_cast<std::uint64_t>(ground.dig_power));
        mix(hash, static_cast<std::uint64_t>(ground.flame_ticks));
        mix(hash, ground.muffled_uses);
        mix(hash, static_cast<std::uint64_t>(ground.flight.slot));
        mix(hash, ground.flight.generation);
        mix(hash, static_cast<std::uint64_t>(ground.anchor.slot));
        mix(hash, ground.anchor.generation);
    }
    mix(hash, game.flight_contacts.size());
    for (const FlightContact& hit : game.flight_contacts)
        for (Handle handle : {hit.projectile, hit.victim}) {
            mix(hash, static_cast<std::uint64_t>(handle.slot));
            mix(hash, handle.generation);
        }
    mix(hash,game.reactor_front.size());
    for (const ReactorEvent& event:game.reactor_front) {
        mix(hash,event.tile);mix(hash,event.due);mix(hash,event.warned_at);
    }
    mix(hash,game.boiler_feeds.size());
    for (const auto& feed:game.boiler_feeds) {
        mix(hash,static_cast<std::uint64_t>(feed.tank.slot));mix(hash,feed.tank.generation);
        for (Cell cell:{feed.mount,feed.source,feed.delivery}) {
            mix(hash,static_cast<std::uint64_t>(cell.x));mix(hash,static_cast<std::uint64_t>(cell.y));
        }
        mix(hash,feed.water);mix(hash,feed.dry_ticks);
        mix(hash,static_cast<std::uint64_t>(feed.drive.x));mix(hash,static_cast<std::uint64_t>(feed.drive.y));
        mix(hash,static_cast<std::uint64_t>(feed.cutter.slot));mix(hash,feed.cutter.generation);
        mix(hash,feed.belts.size());
        for (Cell cell:feed.belts) {mix(hash,static_cast<std::uint64_t>(cell.x));mix(hash,static_cast<std::uint64_t>(cell.y));}
    }
    mix(hash,game.industrial_shifts.size());
    for (const auto& shift:game.industrial_shifts) {
        for (Handle handle:{shift.tank,shift.foreman,shift.hauler}) {mix(hash,static_cast<std::uint64_t>(handle.slot));mix(hash,handle.generation);}
        for (Cell cell:{shift.origin,shift.direction}) {mix(hash,static_cast<std::uint64_t>(cell.x));mix(hash,static_cast<std::uint64_t>(cell.y));}
    }
    mix(hash,game.lava_vents.size());
    for (const auto& vent:game.lava_vents) {
        for (Cell cell:{vent.source,vent.target}) {mix(hash,static_cast<std::uint64_t>(cell.x));mix(hash,static_cast<std::uint64_t>(cell.y));}
        mix(hash,vent.ticks);mix(hash,static_cast<std::uint64_t>(vent.phase));
    }
    mix(hash,game.fissures.size());
    for (const auto& f:game.fissures) {
        for (Cell cell:{f.center,f.axis}) {mix(hash,static_cast<std::uint64_t>(cell.x));mix(hash,static_cast<std::uint64_t>(cell.y));}
        mix(hash,static_cast<std::uint64_t>(f.kind));mix(hash,static_cast<std::uint64_t>(f.phase));mix(hash,f.ticks);mix(hash,f.heat);
    }
    return hash;
}
