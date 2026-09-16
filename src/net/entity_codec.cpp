#include "../entities/dog.hpp"
#include "../projectiles/arrow_fire.hpp"
#include "../entities/fishing_work.hpp"
#include "../entities/crane_operator.hpp"
#include "../entities/coal_cutter.hpp"
#include "../entities/brawler.hpp"
#include "../entities/zombie.hpp"
#include "../entities/wolf.hpp"
#include "../items/cooking.hpp"
#include "../entities/rail_cart.hpp"
#include "../entities/rail_shunter.hpp"
#include "../projectiles/tar_spit.hpp"
#include "../entities/tar_choir.hpp"
#include "../items/heated_water.hpp"
#include "../entities/mold_thief.hpp"
#include "../entities/casting_mold.hpp"
#include "../items/machine_fittings.hpp"
#include "../entities/emergency_pump.hpp"
#include "../items/pocket_pump.hpp"
#include "../entities/counterweight.hpp"
#include "../entities/ash_sleeper.hpp"
#include "../traps/nail_board.hpp"
#include "../projectiles/chain_hook.hpp"
#include "../entities/slag_snail.hpp"
#include "../items/emergency_foam.hpp"
#include "../entities/furnace_moth.hpp"
#include "../entities/audit_clerk.hpp"
#include "../entities/walking_kiln.hpp"
#include "../items/pocket_drill.hpp"
#include "../entities/cable_crawler.hpp"
#include "../entities/pressure_rat.hpp"
#include "../entities/magnet_crane.hpp"
#include "../entities/arc_welder.hpp"
#include "../items/arc_torch.hpp"
#include "../combat/toss.hpp"
#include "../entities/yeti.hpp"
#include "../entities/strikebreaker.hpp"
#include "../items/rivet_gun.hpp"
#include "../entities/rivet_gunner.hpp"
#include "../entities/ember.hpp"
#include "../entities/powder_monkey.hpp"
#include "../items/quarry_charge.hpp"
#include "../entities/hearing.hpp"
#include "../items/sled.hpp"
#include "../items/ice_anchor.hpp"
#include "../items/thaw_charge.hpp"
#include "../entities/gate.hpp"
#include "../projectiles/harpoon.hpp"
#include "../items/echo_pebble.hpp"
#include "../projectiles/projectile.hpp"
#include "entity_codec.hpp"
#include "../items/bolt_pouch.hpp"
#include "../entities/boiler_tank.hpp"
#include "../items/flare.hpp"
#include "../items/muffling.hpp"
#include "../props/candle.hpp"

namespace {

// ITEMS: Carried and ground items share field order and validation.
void write_item(PacketWriter& writer, const Item& item) {
    writer.u8(static_cast<std::uint8_t>(item.kind));
    writer.u8(static_cast<std::uint8_t>(item.attribute));
    writer.i32(item.count); writer.i32(item.max_count);
    writer.u8(static_cast<std::uint8_t>(item.consume_on_use));
    writer.i32(item.cooldown); writer.i32(item.loaded);
    writer.i32(item.spare); writer.i32(item.durability);
    writer.i32(item.max_durability); writer.i32(item.uses); writer.i32(item.max_uses);
    writer.u8(static_cast<std::uint8_t>(item.opened));
    writer.light(item.light);
    writer.i32(item.dig_power);
    writer.i32(item.flame_ticks);
    writer.u8(item.muffled_uses);
    writer.i32(item.flight.slot); writer.u32(item.flight.generation);
    writer.i32(item.anchor.slot); writer.u32(item.anchor.generation);
}
Item read_item(PacketReader& reader) {
    Item item;
    const std::uint8_t kind = reader.u8();
    if (kind >= static_cast<std::uint8_t>(ItemKind::Count)) reader.okay = false;
    item.kind = static_cast<ItemKind>(kind);
    item.attribute = static_cast<ItemAttribute>(reader.u8());
    item.count = reader.i32(); item.max_count = reader.i32();
    item.consume_on_use = reader.u8() != 0;
    item.cooldown = reader.i32(); item.loaded = reader.i32();
    item.spare = reader.i32(); item.durability = reader.i32();
    item.max_durability = reader.i32(); item.uses = reader.i32();
    item.max_uses = reader.i32(); item.opened = reader.u8() != 0;
    item.light = reader.light();
    item.dig_power = reader.i32();
    item.flame_ticks = reader.i32();
    item.muffled_uses = reader.u8();
    if (item.muffled_uses > 6 || (item.muffled_uses > 0 && !muffleable_item(item))) reader.okay = false;
    item.flight = {reader.i32(), reader.u32()};
    item.anchor = {reader.i32(), reader.u32()};
    if (item.anchor.slot < -1 || item.anchor.slot >= max_entities ||
        (item.anchor.slot >= 0 && item.kind != ItemKind::PocketDoor && item.kind != ItemKind::IceAnchor)) reader.okay = false;
    if (item.flight.slot < -1 || item.flight.slot >= max_entities ||
        (item.flight.slot >= 0 && item.kind != ItemKind::Boomerang && item.kind != ItemKind::HarpoonGun && item.kind != ItemKind::ChainHook)) reader.okay = false;
    if (item.kind==ItemKind::LunchTin && (item.loaded>2 || item.spare!=0 || item.count>1)) reader.okay=false;
    if (item.kind==ItemKind::GlowSlag && (item.loaded>1200 || item.spare!=0 || item.count>1)) reader.okay=false;
    if (item.kind==ItemKind::EffigyMask && (item.spare>59 || item.loaded!=0)) reader.okay=false;
    if (!valid_nozzle_elbow(item)) reader.okay=false;
    if (!valid_pocket_pump(item)) reader.okay=false;
    if (item.kind==ItemKind::HeatSiphon && (item.loaded>1800 || item.spare!=0)) reader.okay=false;
    if (item.kind == ItemKind::HarpoonGun && item.loaded > 1) reader.okay = false;
    if (item.kind == ItemKind::EchoPebble) {
        const EchoVoice* voice = echo_voice(item);
        if ((item.loaded != 0 && voice == nullptr) || item.spare != (voice ? voice->radius : 0)) reader.okay = false;
    }
    if (item.kind==ItemKind::StormLantern && (item.loaded>7200 || item.spare>3 ||
        item.light.shape==LightShape::Omni)) reader.okay=false;
    if (item.flame_ticks < 0 || item.flame_ticks > 1800) reader.okay = false;
    if (item.dig_power < 0 || item.dig_power > 255) reader.okay = false;
    if (item.kind == ItemKind::CandleStub && item.loaded > candle_fuel_ticks) reader.okay = false;
    if (heated_water_item(item.kind) && (item.loaded > 2 ||
        item.spare > (item.loaded == 2 ? 1800 : item.loaded == 1 ? 89 : 0))) reader.okay = false;
    if (item.count < 0 || item.max_count < item.count || item.cooldown < 0 ||
        item.loaded < 0 || item.spare < 0 ||
        item.durability < 0 || item.max_durability < item.durability ||
        item.uses < 0 || item.max_uses < item.uses ||
        item.attribute > ItemAttribute::Restorative) reader.okay = false;
    return item;
}

} // namespace

// ENTITIES: Preserve generation even for vacant slots; occupied slots carry all state.
void write_entity(PacketWriter& writer, const Entity& entity) {
    writer.u8(static_cast<std::uint8_t>(entity.kind));
    writer.u32(entity.generation);
    if (entity.kind == EntityKind::None) return;
    writer.cell(entity.cell); writer.cell(entity.facing);
    writer.u16(static_cast<std::uint16_t>(entity.sprite));
    writer.light(entity.light);
    writer.u8(entity.self_light.red); writer.u8(entity.self_light.green);
    writer.u8(entity.self_light.blue);
    writer.i32(entity.owner); writer.i32(entity.health); writer.i32(entity.max_health);
    writer.i32(entity.move_wait); writer.i32(entity.move_interval);
    writer.i32(entity.attack_wait); writer.i32(entity.attack_interval);
    writer.i32(entity.use_flash); writer.i32(entity.block_ticks);
    writer.i32(entity.guard_slot);
    writer.i32(entity.burn_ticks); writer.i32(entity.freeze_ticks);
    writer.i32(entity.scorch_ticks); writer.i32(entity.fire_dim_ticks);
    writer.u8(entity.fire_tramples);
    writer.i32(entity.sleep_ticks); writer.i32(entity.stun_ticks);
    writer.u16(entity.vitals.healing_left); writer.u16(entity.vitals.healing_wait);
    writer.u8(static_cast<std::uint8_t>(entity.vitals.recovery)); writer.u16(entity.vitals.chill_guard);
    writer.u16(entity.vitals.summer_ticks); writer.u8(entity.vitals.summer_radius);
    writer.u16(entity.vitals.sleep_guard); writer.u16(entity.vitals.stun_guard);
    writer.u16(entity.vitals.haste); writer.u16(entity.vitals.rooted);
    writer.u16(entity.vitals.nausea); writer.u16(entity.vitals.nausea_wait);
    writer.u16(entity.vitals.floor_insulation);
    writer.u16(entity.vitals.traction); writer.u16(entity.vitals.slide_momentum);
    writer.u16(entity.vitals.grip); writer.u8(static_cast<std::uint8_t>(entity.vitals.root_kind));
    writer.cell(entity.toss.origin); writer.cell(entity.toss.direction); writer.cell(entity.toss.source);
    writer.i32(entity.toss.instigator.slot); writer.u32(entity.toss.instigator.generation);
    writer.i32(entity.toss.ticks);
    writer.i32(entity.script_tick); writer.u32(entity.artifacts);
    writer.i32(entity.train_cars_left); writer.i32(entity.spawn_wait);
    writer.cell(entity.train_origin);
    for (Handle handle : {entity.entity_a, entity.entity_b, entity.encounter}) {
        writer.i32(handle.slot); writer.u32(handle.generation);
    }
    writer.cell(entity.point_a); writer.cell(entity.point_b); writer.cell(entity.point_c);
    for (int value : {entity.counter_a, entity.counter_b, entity.counter_c, entity.label_a, entity.label_b, entity.label_c,
                      entity.timer_a, entity.timer_b, entity.timer_c}) writer.i32(value);
    writer.u64(entity.birth_tick);
    writer.u8(static_cast<std::uint8_t>(entity.impassable));
    writer.u8(static_cast<std::uint8_t>(entity.hard_blocker));
    writer.u8(static_cast<std::uint8_t>(entity.fixture_open));
    writer.i32(entity.inventory.selected);
    for (const Item& item : entity.inventory.slots) write_item(writer, item);
    write_item(writer, entity.ground_item);
}

Entity read_entity(PacketReader& reader) {
    Entity entity;
    const std::uint8_t kind = reader.u8();
    if (kind >= static_cast<std::uint8_t>(EntityKind::Count)) reader.okay = false;
    entity.kind = static_cast<EntityKind>(kind);
    entity.generation = reader.u32();
    if (entity.kind == EntityKind::None) return entity;
    entity.cell = reader.cell(); entity.facing = reader.cell();
    const std::uint16_t sprite = reader.u16();
    if (sprite >= static_cast<std::uint16_t>(Sprite::Count)) reader.okay = false;
    entity.sprite = static_cast<Sprite>(sprite);
    entity.light = reader.light();
    entity.self_light = {reader.u8(), reader.u8(), reader.u8()};
    entity.owner = reader.i32(); entity.health = reader.i32(); entity.max_health = reader.i32();
    entity.move_wait = reader.i32(); entity.move_interval = reader.i32();
    entity.attack_wait = reader.i32(); entity.attack_interval = reader.i32();
    entity.use_flash = reader.i32(); entity.block_ticks = reader.i32();
    entity.guard_slot = reader.i32();
    if (entity.guard_slot < -1 || entity.guard_slot >= quick_slots) reader.okay = false;
    entity.burn_ticks = reader.i32(); entity.freeze_ticks = reader.i32();
    entity.scorch_ticks = reader.i32(); entity.fire_dim_ticks = reader.i32();
    entity.fire_tramples = reader.u8();
    entity.sleep_ticks = reader.i32(); entity.stun_ticks = reader.i32();
    entity.vitals.healing_left = reader.u16(); entity.vitals.healing_wait = reader.u16();
    entity.vitals.recovery = static_cast<RecoveryKind>(reader.u8()); entity.vitals.chill_guard = reader.u16();
    entity.vitals.summer_ticks=reader.u16(); entity.vitals.summer_radius=reader.u8();
    if (entity.vitals.summer_ticks>240 || entity.vitals.summer_radius>2 ||
        ((entity.vitals.summer_ticks==0)!=(entity.vitals.summer_radius==0))) reader.okay=false;
    entity.vitals.sleep_guard = reader.u16(); entity.vitals.stun_guard = reader.u16();
    entity.vitals.haste = reader.u16(); entity.vitals.rooted = reader.u16();
    entity.vitals.nausea = reader.u16(); entity.vitals.nausea_wait = reader.u16();
    if (entity.vitals.nausea > 600 || entity.vitals.nausea_wait > 60) reader.okay = false;
    entity.vitals.floor_insulation=reader.u16();
    if (entity.vitals.floor_insulation>240) reader.okay=false;
    entity.vitals.traction = reader.u16(); entity.vitals.slide_momentum = reader.u16();
    if (entity.vitals.traction > 300 || entity.vitals.slide_momentum > 12) reader.okay = false;
    entity.vitals.grip = reader.u16(); entity.vitals.root_kind = static_cast<RootKind>(reader.u8());
    if (entity.vitals.healing_left > 1000 || entity.vitals.healing_wait > recovery_interval(entity.vitals) ||
        entity.vitals.recovery > RecoveryKind::Meal || entity.vitals.chill_guard > 480 ||
        entity.vitals.sleep_guard > 600 || entity.vitals.stun_guard > 180 ||
        entity.vitals.haste > 240 || entity.vitals.rooted > root_tick_limit(entity.vitals.root_kind) || entity.vitals.grip > 360 || entity.vitals.root_kind > RootKind::Net) reader.okay = false;
    entity.toss.origin=reader.cell(); entity.toss.direction=reader.cell(); entity.toss.source=reader.cell();
    entity.toss.instigator={reader.i32(),reader.u32()}; entity.toss.ticks=reader.i32();
    entity.script_tick = reader.i32(); entity.artifacts = reader.u32();
    entity.train_cars_left = reader.i32(); entity.spawn_wait = reader.i32();
    entity.train_origin = reader.cell();
    entity.entity_a = {reader.i32(), reader.u32()};
    entity.entity_b = {reader.i32(), reader.u32()};
    entity.encounter = {reader.i32(), reader.u32()};
    for (Handle handle : {entity.entity_a, entity.entity_b, entity.encounter})
        if (handle.slot < -1 || handle.slot >= max_entities) reader.okay = false;
    entity.point_a = reader.cell(); entity.point_b = reader.cell(); entity.point_c = reader.cell();
    entity.counter_a = reader.i32(); entity.counter_b = reader.i32(); entity.counter_c = reader.i32();
    entity.label_a = reader.i32(); entity.label_b = reader.i32(); entity.label_c = reader.i32();
    entity.timer_a = reader.i32(); entity.timer_b = reader.i32(); entity.timer_c = reader.i32();
    if (entity.label_c < 0 || entity.label_c > PlayerWorkOrder || entity.timer_c < 0 || entity.timer_c > 300) reader.okay = false;
    if (entity.timer_a < 0 || entity.timer_b < 0) reader.okay = false;
    if (entity.kind == EntityKind::IcicleSpider && (entity.label_a < 0 || entity.label_a > 5 ||
        entity.counter_a < 0 || entity.counter_a > 1)) reader.okay = false;
    entity.birth_tick = reader.u64();
    entity.impassable = reader.u8() != 0;
    entity.hard_blocker = reader.u8() != 0;
    entity.fixture_open = reader.u8() != 0;
    entity.inventory.selected = reader.i32();
    if (entity.inventory.selected < 0 || entity.inventory.selected >= quick_slots)
        reader.okay = false;
    for (Item& item : entity.inventory.slots) item = read_item(reader);
    entity.ground_item = read_item(reader);
    if (entity.kind==EntityKind::Projectile && entity.label_a==static_cast<int>(ProjectileKind::Arrow) &&
        (entity.counter_c<0 || entity.counter_c>1 || (entity.counter_c==1 && !wooden_arrow(entity)))) reader.okay=false;
    if (entity.kind == EntityKind::Projectile && entity.label_a == static_cast<int>(ProjectileKind::EchoPebble) &&
        (entity.ground_item.kind != ItemKind::EchoPebble || entity.label_b < 0 || entity.label_b > 1 ||
         entity.counter_a < 0 || entity.counter_a > 20 || entity.counter_b < 0 || entity.counter_b > 3 ||
         entity.timer_a < 0 || entity.timer_a > 240 || entity.timer_b < 0 || entity.timer_b > 6)) reader.okay = false;
    if (entity.kind==EntityKind::PowderMonkey && (entity.label_a<PowderReady || entity.label_a>PowderRest ||
        entity.counter_a<0 || entity.counter_a>2)) reader.okay=false;
    if (entity.kind==EntityKind::Ember && (entity.label_a<StokerReady || entity.label_a>StokerRest ||
        entity.counter_a<0 || entity.counter_a>5 || entity.counter_b<0 || entity.counter_b>1 ||
        entity.label_b<0 || entity.label_b>1)) reader.okay=false;
    if (entity.kind==EntityKind::Projectile && entity.label_a==static_cast<int>(ProjectileKind::CoalSpit) &&
        (entity.label_b<0 || entity.label_b>1 || entity.counter_a<0 || entity.counter_a>8 ||
         entity.counter_b!=(entity.label_b==1 ? 12 : 4) || entity.timer_c>164)) reader.okay=false;
    if (!valid_coal_cutter(entity)) reader.okay=false;
    if (!valid_rail_cart(entity) || !valid_rail_shunter(entity)) reader.okay=false;
    if (!valid_tar_singer(entity) || !valid_tar_spit(entity)) reader.okay=false;
    if (!valid_mold_thief(entity) || !valid_casting_mold(entity)) reader.okay=false;
    if (!valid_machine_fitting(entity)) reader.okay=false;
    if (!valid_emergency_pump(entity)) reader.okay=false;
    if (!valid_counterweight(entity)) reader.okay=false;
    if (!valid_ash_sleeper(entity)) reader.okay=false;
    if (!valid_slag_snail(entity)) reader.okay=false;
    if (!valid_furnace_moth(entity)) reader.okay=false;
    if (!valid_audit_clerk(entity)) reader.okay=false;
    if (!valid_walking_kiln(entity)) reader.okay=false;
    if (!valid_cable_crawler(entity)) reader.okay=false;
    if (!valid_pressure_rat(entity)) reader.okay=false;
    if (!valid_fishing_widow(entity)) reader.okay=false;
    if (!valid_crane_operator(entity)) reader.okay=false;
    if (!valid_magnet_crane(entity)) reader.okay=false;
    if (!valid_arc_welder(entity)) reader.okay=false;
    if (!valid_pocket_drill(entity)) reader.okay=false;
    if (!valid_arc_torch(entity)) reader.okay=false;
    if (!valid_brawler(entity)) reader.okay=false;
    if (!valid_zombie_swipe(entity)) reader.okay=false;
    if (!valid_wolf_bite(entity) || !valid_dog_bite(entity)) reader.okay=false;
    if (!valid_cooking_state(entity)) reader.okay=false;
    if (!valid_actor_toss(entity) || !valid_yeti(entity)) reader.okay=false;
    if (!valid_rivet_gunner(entity)) reader.okay=false;
    if (!valid_rivet_action(entity)) reader.okay=false;
    if (!valid_strikebreaker(entity)) reader.okay=false;
    if (!valid_sled(entity)) reader.okay=false;
    if (!valid_ice_anchor(entity)) reader.okay=false;
    if (!valid_quarry_charge(entity)) reader.okay=false;
    if (!valid_foam_can(entity)) reader.okay=false;
    if (!valid_nail_board(entity)) reader.okay=false;
    if (!valid_chain_hook(entity)) reader.okay=false;
    if (!valid_thrown_bolt(entity)) reader.okay=false;
    if (!valid_thaw_charge(entity)) reader.okay=false;
    if (!valid_boiler_state(entity) || !valid_flare_state(entity) || !valid_harpoon_state(entity) || !valid_gate_state(entity)) reader.okay = false;
    if (entity.health < 0 || entity.max_health < 0 || entity.move_wait < 0 ||
        entity.move_interval < 0 || entity.attack_wait < 0 || entity.attack_interval < 0 ||
        entity.spawn_wait < 0 || entity.owner >= 4 || entity.burn_ticks < 0 ||
        entity.freeze_ticks < 0 || entity.sleep_ticks < 0 || entity.stun_ticks < 0 ||
        entity.scorch_ticks < 0 || entity.scorch_ticks > 300 || entity.fire_tramples > 5 ||
        entity.fire_dim_ticks < 0 || entity.fire_dim_ticks > 60)
        reader.okay = false;
    return entity;
}
