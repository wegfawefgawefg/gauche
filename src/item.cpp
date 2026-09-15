#include "items/coolant.hpp"
#include "items/barricade.hpp"
#include "items/magnet.hpp"
#include "items/belt_tools.hpp"
#include "artifacts/hearth.hpp"
#include "items/ammunition.hpp"
#include "items/foreman_whistle.hpp"
#include "items/quarry_charge.hpp"
#include "items/fuse_scissors.hpp"
#include "items/sled.hpp"
#include "items/snow_shelter.hpp"
#include "items/stillwater_bell.hpp"
#include "items/tuning_fork.hpp"
#include "items/borrowed_summer.hpp"
#include "props/doorstop.hpp"
#include "items/echo_pebble.hpp"
#include "items/circuits.hpp"
#include "items/flare.hpp"
#include "items/ice_equipment.hpp"
#include "game.hpp"
#include "items/heat_siphon.hpp"
#include "items/thaw_charge.hpp"
#include "items/folded_bridge.hpp"
#include "props/candle.hpp"
#include "items/coal.hpp"
#include "items/kettle.hpp"
#include "items/pressure.hpp"
#include "items/fish.hpp"
#include "items/snow_globe.hpp"
#include "items/optics.hpp"
#include "items/muffling.hpp"
#include "props/cloth.hpp"
#include "props/alarm_clock.hpp"
#include "projectiles/fishing.hpp"
#include "projectiles/projectile.hpp"
#include "items/catalog.hpp"
#include "items/eel_battery.hpp"
#include "items/snow_tools.hpp"
#include "projectiles/snowball.hpp"
#include "projectiles/prism.hpp"
#include "items/air_bladder.hpp"
#include "items/heat_capsule.hpp"
#include "items/cold_remedies.hpp"
#include "items/woodland_tools.hpp"
#include "items/remedies.hpp"
#include "items/ground_tools.hpp"
#include "items/displacement.hpp"
#include "items/root_relics.hpp"
#include "items/woodland_traps.hpp"
#include "items/movement_tools.hpp"
#include "items/noisemakers.hpp"
#include "items/mixtures.hpp"
#include "items/decoys.hpp"
#include "items/pocket_door.hpp"
#include "items/ice_footing.hpp"
#include "projectiles/hook.hpp"
#include "projectiles/thunder.hpp"
#include "projectiles/recoverable.hpp"
#include "items/materials.hpp"
#include "items/firearms.hpp"
#include "item_attribute.hpp"
#include "combat/shove.hpp"
#include "combat/parry.hpp"
#include "entities/dispatch.hpp"
#include "entities/behavior.hpp"
#include "item_pattern.hpp"
#include "world/ground_items.hpp"
#include "props/interaction.hpp"

#include <algorithm>
#include <cstdlib>

namespace {

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
    if (item.kind == ItemKind::None || item.count <= 0 || item.cooldown > 0 || item.flight.slot >= 0) return false;
    const Cell direction = cardinal_toward(user.cell, target, user.facing);
    const ItemKind used_kind = item.kind;
    user.facing = direction;
    const int range = distance(user.cell, target);
    bool used = false;
    int cooldown = 0;
    switch (item.kind) {
    case ItemKind::CoolantCan:
        used=pour_coolant(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::FoldingBarricade:
        used=place_barricade(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::HorseshoeMagnet:
        used=pull_magnetic_item(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::BeltCrank: case ItemKind::BrakeShoe:
        used=use_belt_tool(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::QuarryCharge:
        used=place_quarry_charge(game,user_slot); cooldown=30; break;
    case ItemKind::FuseScissors:
        used=snip_fuse(game,user_slot,direction); cooldown=24; break;
    case ItemKind::ForemanWhistle:
        used=use_foreman_whistle(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::StillwaterBell:
        used=ring_stillwater_bell(game,user_slot); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::TuningFork:
        used=ring_tuning_fork(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::FoldedBridge:
        used=place_folded_bridge(game,user_slot); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::ThawCharge:
        used=place_thaw_charge(game,user_slot); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::HeatSiphon: return draw_siphon_heat(game,user_slot);
    case ItemKind::BorrowedSummer:
        used = use_borrowed_summer(game,user_slot); cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::EmergencyDoorstop:
        used = place_doorstop(game,user.cell+direction,item); cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::EchoPebble:
        used = launch_echo_pebble(game,user_slot,item,direction); cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::StormLantern: break; // Its shutter/focus follows held input in the player step.
    case ItemKind::CopperWire: case ItemKind::GroundingSpike:
        used = place_circuit_item(game,user.cell+direction,item); cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::SignalFlare:
        used = launch_flare(game,user_slot,item,direction); cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::Crampons:
        used = use_crampons(user); cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::PressureValve: case ItemKind::Sealant:
        cooldown = item_pattern(item).cooldown;
        used = use_pressure_item(game,user_slot,direction);
        break;
    case ItemKind::SteamKettle:
        cooldown = item_pattern(item).cooldown;
        used = use_kettle(game,user_slot,direction);
        break;
    case ItemKind::CoalLump:
        used = use_coal(game,user_slot,direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::CandleStub:
        used = place_candle(game, user.cell + direction, item);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::WickSpool:
        used = refill_candle(game, user.cell + direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::SaltedKelp:
        return eat_held_kelp(game, user_slot);
    case ItemKind::SnowGlobe:
        used = use_snow_globe(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::FishingLine:
        used = launch_fishing_hook(game, user_slot, item, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::AlarmClock:
        used = place_alarm_clock(game, user.cell + direction, item);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::MufflingFelt:
        used = apply_muffling(game, user_slot);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::BlackFelt:
        used = cover_optic(game, user.cell + direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::PrismBomb:
        used = launch_prism_bomb(game, user_slot, item, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::MirrorShard: case ItemKind::CrystalLens:
        used = place_optic(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::WoolWrap: case ItemKind::HotBroth: case ItemKind::IcePoultice:
        used = use_cold_remedy(game, user_slot);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::HeatCapsule:
        used = use_heat_capsule(game, user_slot);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::BrineFlask: case ItemKind::ColdFlask:
        used = launch_projectile(game, user_slot, item, direction, item_pattern(item).maximum);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::AirBladder:
        used = use_air_bladder(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::GritPouch:
        used = scatter_grit(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::PocketDoor:
        used = place_pocket_door(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::StrawDecoy: case ItemKind::Scarecrow:
        used = place_decoy(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::StinkBomb: case ItemKind::RottenFruit: case ItemKind::PitchBomb:
        used = throw_mixture(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::WolfWhistle: case ItemKind::HandBell: case ItemKind::Firecracker:
        used = use_noisemaker(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::ThrowingNet: case ItemKind::StickyBoots: case ItemKind::RabbitCharm:
        used = use_movement_tool(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::RopeSnare: case ItemKind::SpringTrap: case ItemKind::AcornMine:
        used = place_woodland_trap(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::ThunderAcorn:
        used = launch_thunder_acorn(game, user_slot, item, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::RootDrill: case ItemKind::BlinkSeed:
        used = launch_root_relic(game, user_slot, item, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::HuntingHorn:
        used = blow_hunting_horn(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::RopeHook:
        used = launch_hook(game, user_slot, item, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::BirdSeed: case ItemKind::ThornCaltrops:
        used = use_ground_tool(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::HerbBag: case ItemKind::Splint: case ItemKind::BitterRoot:
    case ItemKind::Chili: case ItemKind::FungalBread:
        used = use_remedy(game, user_slot);
        cooldown = item_pattern(item).cooldown;
        break;
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
    case ItemKind::SmokedFish: case ItemKind::Egg: case ItemKind::FriedEgg:
        if (user.health > 0 && (user.health < user.max_health || hearth_meal_needed(game,user,used_kind))) {
            const ItemPattern pattern = item_pattern(item);
            user.health = std::min(user.max_health, user.health + pattern.heal);
            used = true;
            cooldown = pattern.cooldown;
        }
        break;
    case ItemKind::SnowScoop:
        used = use_snow_scoop(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::Snowball:
        used = launch_snowball(game, user_slot, item, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::EelBattery:
        used = use_eel_battery(game, user_slot, direction);
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::PressHammer: case ItemKind::RubberMallet:
    case ItemKind::SkateBlade: case ItemKind::Chisel:
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
    case ItemKind::ReflectingPan:
        user.guard_slot = user.inventory.selected;
        user.block_ticks = parry_ticks;
        used = true;
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::ShieldLantern: case ItemKind::Buckler:
        user.guard_slot = user.inventory.selected;
        user.block_ticks = 15;
        shove_in_front(game, user_slot, direction);
        used = true;
        cooldown = item_pattern(item).cooldown;
        break;
    case ItemKind::HarpoonGun: case ItemKind::LensCarbine:
    case ItemKind::Crossbow: case ItemKind::Blunderbuss:
    case ItemKind::Pistol: case ItemKind::Musket:
    case ItemKind::RocketLauncher: case ItemKind::Shotgun: case ItemKind::SMG:
        used = fire_weapon(game, user_slot, direction, item);
        return used;
    case ItemKind::IceAnchor: return false; // Placement/reeling belongs to the player action.
    case ItemKind::Sled:
        used=place_sled(game,user_slot,direction,item);
        cooldown=item_pattern(item).cooldown;
        break;
    case ItemKind::SnowShelter:
        used=place_snow_shelter(game,user_slot,direction);
        cooldown=item_pattern(item).cooldown;
        break;
    case ItemKind::EffigyMask: return false; // Continuous stationary use belongs to the player action.
    case ItemKind::IceBrick: return false; // Tap/hold release is owned by the player action step.
    case ItemKind::RivetGun: return false; // The timed burst belongs to the player action.
    case ItemKind::Bow: return false; // Draw/release is handled by the player action step.
    case ItemKind::IceNeedle: case ItemKind::Boomerang: case ItemKind::ThrowingRock:
        used = launch_recoverable(game, user_slot, item, direction);
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
                used = apply_sleep(sleeper, 180);
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
        used = supply_ammunition(user.inventory);
        break;
    case ItemKind::PocketDrill:
    case ItemKind::ArcTorch: // Continuous action owns priming and battery use in step_player.
    case ItemKind::Count: case ItemKind::None:
        break;
    }
    if (used) {
        share_hearth_meal(game,user,used_kind);
        if (item_is_melee(used_kind)) finish_muffled_use(game, item, user.cell);
        item.cooldown = cooldown;
        user.use_flash = 8;
        if (const RegionalItem* spec = regional_item(used_kind)) {
            if (used_kind == ItemKind::CandleStub)
                emit_sound(game, item.loaded > 0 ? SoundId::CandleLight : SoundId::Drop, user.cell + direction);
            else if (!item_is_melee(used_kind) && used_kind != ItemKind::CoalLump && used_kind != ItemKind::SteamKettle) emit_sound(game, spec->sound,
                used_kind == ItemKind::SnowGlobe ? user.cell + direction : user.cell);
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
        if ((used_kind == ItemKind::PressHammer || used_kind == ItemKind::RubberMallet) && --item.durability <= 0) {
            emit_sound(game,SoundId::WoodCrack,user.cell);
            item = {};
            return true;
        }
        if ((used_kind == ItemKind::SkateBlade || used_kind == ItemKind::Chisel || used_kind == ItemKind::SnowScoop) && --item.durability <= 0) {
            emit_sound(game, used_kind == ItemKind::SkateBlade ? SoundId::SkateBreak : used_kind == ItemKind::SnowScoop ? SoundId::ScoopBreak : SoundId::ChiselBreak, user.cell);
            item = {};
            return true;
        }
        if (item.max_uses > 0 && --item.uses <= 0) {
            if (used_kind == ItemKind::CoolantCan) emit_sound(game,SoundId::CoolantEmpty,user.cell);
            else if (used_kind == ItemKind::HorseshoeMagnet) emit_sound(game,SoundId::MagnetSpent,user.cell);
            else if (used_kind == ItemKind::SnowShelter) emit_sound(game,SoundId::ShelterEmpty,user.cell);
            else if (used_kind == ItemKind::FuseScissors) emit_sound(game,SoundId::ScissorsSpent,user.cell);
            else if (used_kind == ItemKind::ForemanWhistle) emit_sound(game,SoundId::WorkSpent,user.cell);
            else if (used_kind == ItemKind::StillwaterBell) emit_sound(game,SoundId::StillwaterSpent,user.cell);
            else if (used_kind == ItemKind::TuningFork) emit_sound(game,SoundId::ForkSpent,user.cell);
            else if (used_kind == ItemKind::CopperWire) emit_sound(game,SoundId::WireEmpty,user.cell);
            else if (used_kind == ItemKind::Crampons) emit_sound(game,SoundId::CramponsSpent,user.cell);
            else if (used_kind == ItemKind::Sealant) emit_sound(game,SoundId::SealantEmpty,user.cell);
            else if (used_kind == ItemKind::WickSpool) emit_sound(game, SoundId::WickEmpty, user.cell);
            else if (used_kind == ItemKind::FishingLine) emit_sound(game, SoundId::FishingEmpty, user.cell);
            else if (used_kind == ItemKind::MufflingFelt) emit_sound(game, SoundId::MuffleEmpty, user.cell);
            else if (used_kind == ItemKind::EelBattery) emit_sound(game, SoundId::BatteryEmpty, user.cell);
            else if (used_kind == ItemKind::AirBladder) emit_sound(game, SoundId::AirEmpty, user.cell);
            else if (used_kind == ItemKind::GritPouch) emit_sound(game, SoundId::GritEmpty, user.cell);
            else if (used_kind != ItemKind::PocketDoor && used_kind != ItemKind::BorrowedSummer)
                emit_sound(game, SoundId::BoxBreak, user.cell);
            item = {};
            return true;
        }
        if (item.consume_on_use && --item.count <= 0) item = {};
    }
    return used;
}
