#include "light_tower.hpp"
#include "tall_tree.hpp"
#include "ice_pillar.hpp"
#include "../status/bleeding.hpp"
#include "../world/water.hpp"
#include "../entities/audit_clerk.hpp"
#include "tension_spring.hpp"
#include "streetlamp.hpp"
#include "interaction.hpp"
#include "../items/supply.hpp"
#include "conveyor.hpp"
#include "../items/folded_bridge.hpp"
#include "../entities/icicle_spider.hpp"
#include "candle.hpp"
#include "stove.hpp"
#include "cloth.hpp"
#include "../combat/beams.hpp"
#include "../world/ground_items.hpp"
#include "../world/loot.hpp"

#include <algorithm>

namespace {

void drop_contents(Game& game, Cell cell, PropKind kind) {
    // LOOT: Empty clutter is normal; nests and containers have contextual finds.
    ItemKind item = ItemKind::None;
    const std::uint32_t roll = random_u32(game) % 100;
    switch (kind) {
    case PropKind::ChapelAltar:
        if (roll < 35) item=ItemKind::WoolWrap;
        else if (roll < 65) item=ItemKind::HotBroth;
        else {place_ground_item(game,cell,ItemKind::Bandage,2);return;}
        break;
    case PropKind::ChapelUrn:
        if (roll < 25) item=ItemKind::Bandage;
        else if (roll < 50) item=ItemKind::WickSpool;
        else place_coins(game,cell,2+static_cast<int>(random_u32(game)%4));
        break;
    case PropKind::ScrapBin:
        if (roll<20) item=ItemKind::HorseshoeMagnet;
        else if (roll<40) item=ItemKind::ChainHook;
        else if (roll>=40 && roll<60) item=ItemKind::CopperWire;
        else if (roll>=60 && roll<80) {place_ground_item(game,cell,ItemKind::BoltPouch,3);return;}
        break; // Unimplemented slag/hook ranges remain empty.
    case PropKind::OreBin:
        if (roll<25) { place_ground_item(game,cell,ItemKind::CoalLump,2); return; }
        if (roll<40) place_coins(game,cell,2+static_cast<int>(random_u32(game)%3));
        else if (roll<55) item=ItemKind::CoolantCan;
        break;
    case PropKind::MaintenanceLocker:
        if (roll < 20) item = ItemKind::CoalLump;
        else if (roll < 40) item = ItemKind::Sealant;
        else if (roll < 55) item = ItemKind::PressureValve;
        else if (roll < 70) item = ItemKind::Ammo;
        else if (roll < 80) item = ItemKind::CopperWire;
        else if (roll < 88) item = ItemKind::GroundingSpike;
        else if (roll < 92) item = ItemKind::EmergencyDoorstop;
        else if (roll < 95) item = ItemKind::HeatSiphon;
        else if (roll < 98) item = ItemKind::ThawCharge;
        break;
    case PropKind::CandleCabinet:
        if (roll < 35) item = ItemKind::CandleStub;
        else if (roll < 55) item = ItemKind::WickSpool;
        else if (roll < 70) item = ItemKind::WoolWrap;
        else if (roll < 80) item = ItemKind::StormLantern;
        else if (roll < 85) item = ItemKind::BorrowedSummer;
        break;
    case PropKind::FrozenLunchTin:
        if (roll < 30) item = ItemKind::HotBroth;
        else if (roll < 50) item = ItemKind::SaltedKelp;
        else if (roll < 70) item = ItemKind::IcePoultice;
        break;
    case PropKind::FishingCreel:
        if (roll < 35) item = ItemKind::SmokedFish;
        else if (roll < 55) item = ItemKind::FishingLine;
        else if (roll < 60) item = ItemKind::AirBladder;
        else if (roll < 70) item = ItemKind::TuskPike;
        else if (roll < 80) item = ItemKind::FoldedBridge;
        else if (roll < 90) item = ItemKind::Sled;
        break;
    case PropKind::LensCase:
        if (roll < 30) item = ItemKind::MirrorShard;
        else if (roll < 50) item = ItemKind::CrystalLens;
        else if (roll < 65) item = ItemKind::BlackFelt;
        else if (roll < 80) place_coins(game, cell, 3 + static_cast<int>(random_u32(game) % 4));
        else if (roll < 90) item = ItemKind::TuningFork;
        break;
    case PropKind::SnowCache:
        if (roll < 20) item = ItemKind::Snowball;
        else if (roll < 40) item = ItemKind::WoolWrap;
        else if (roll < 55) item = ItemKind::Ammo;
        else if (roll < 75) {
            const auto gear=roll_item_supply(game,LootSource::Cache,false,ItemKind::None,SupplyNeed::Equipment);
            if (gear!=ItemKind::None) place_ground_item(game,cell,gear,supply_count(gear));
            return;
        }
        else if (roll < 90) place_coins(game, cell, 3);
        else item=ItemKind::SnowShelter;
        break;
    case PropKind::RottenLog: if (roll < 15) item = ItemKind::RottenFruit; break;
    case PropKind::Nest: if (roll < 18) item = ItemKind::Egg; break;
    case PropKind::Crate:
        if (roll < 20) item = ItemKind::Ammo;
        else if (roll < 40) item = ItemKind::Bandage;
        else if (roll < 55) {
            const auto gear=roll_item_supply(game,LootSource::Cache,true,ItemKind::None,SupplyNeed::Equipment);
            if (gear!=ItemKind::None) place_ground_item(game,cell,gear,supply_count(gear));
            return;
        }
        else if (roll < 85) place_coins(game, cell, 4 + static_cast<int>(random_u32(game) % 7));
        break;
    case PropKind::ClayPot:
        if (roll < 15) item = ItemKind::Bandaid;
        else if (roll < 45) place_coins(game, cell, 2 + static_cast<int>(random_u32(game) % 5));
        break;
    default: break;
    }
    if (item == ItemKind::None) return;
    const Cell destination = nearby_ground_item_cell(game, cell);
    Entity* dropped = get_entity(game, spawn_entity(game, EntityKind::GroundItem, destination));
    if (dropped == nullptr) return;
    const bool bundle = item == ItemKind::BlackFelt || (kind == PropKind::SnowCache && item == ItemKind::Snowball);
    dropped->ground_item = make_item(item, bundle ? 3 : 1);
    dropped->sprite = item_sprite(item);
}

void break_prop(Game& game, Cell cell, Cell source, Prop& prop) {
    remove_prop_cover(game, cell, false);
    const int deposited=prop.kind==PropKind::PayCage ? prop.growth_ticks : 0;
    prop.hp = 0;
    prop.broken = true;
    prop.growth_ticks = 0;
    if (deposited>0) place_coins(game,cell,deposited);
    if (prop.kind == PropKind::SnowCache && game.stage.at(cell)->kind == TileKind::Snow)
        game.stage.at(cell)->kind = TileKind::Empty;
    const PropSpec spec = prop_spec(prop.kind);
    emit_sound(game, spec.sound, cell);
    if (game.impact_count < static_cast<int>(game.impacts.size()))
        game.impacts[static_cast<std::size_t>(game.impact_count++)] =
            {cell, source, spec.sprite, spec.health, true, prop.kind};
    if (prop.kind == PropKind::Puffball) {
        // SPORES: Breaking a mushroom beside an enemy can buy a short escape.
        for (Entity& actor : game.entities)
            if (actor.health > 0 && actor.move_interval > 0 && !actor.hard_blocker &&
                distance(actor.cell, cell) <= 1)
                apply_sleep(actor, 75);
    }
    if (prop.kind == PropKind::ChapelAltar || prop.kind == PropKind::ChapelUrn || prop.kind == PropKind::ScrapBin || prop.kind == PropKind::OreBin || prop.kind == PropKind::MaintenanceLocker || prop.kind == PropKind::CandleCabinet || prop.kind == PropKind::RottenLog || prop.kind == PropKind::Nest || prop.kind == PropKind::Crate ||
        prop.kind == PropKind::FrozenLunchTin || prop.kind == PropKind::FishingCreel || prop.kind == PropKind::ClayPot || prop.kind == PropKind::SnowCache || prop.kind == PropKind::LensCase) drop_contents(game, cell, prop.kind);
    if (bridge_prop(prop.kind)) collapse_bridge_plank(game,cell,source);
}

} // namespace

bool place_prop(Stage& stage, Cell cell, PropKind kind, std::uint8_t variant) {
    if (bridge_prop(kind)) return false; // Requires a complete supported span.
    Tile* tile = stage.at(cell);
    if (tile == nullptr || !walkable(tile->kind) || tile->prop.kind != PropKind::None)
        return false;
    tile->prop = {kind, static_cast<std::uint8_t>(prop_spec(kind).health), variant, false};
    if (kind==PropKind::Pallet || kind==PropKind::PalletStack || kind==PropKind::BoundRocks || kind==PropKind::ContainerSide) tile->prop.variant &= 1U;
    if (kind==PropKind::IcePillar) tile->prop.hp=static_cast<std::uint8_t>(prop_max_health(tile->prop));
    if (kind==PropKind::IceSpikes || kind==PropKind::SnowPile) tile->prop.variant%=3;
    if (kind==PropKind::StreetLamp) tile->prop.variant &= 3U;
    if (kind==PropKind::PoleWreck) tile->prop.variant &= 1U;
    if (kind == PropKind::RailPoints) tile->prop.variant &= 3U;
    if (kind == PropKind::Conveyor) tile->prop.variant &= 7U;
    if (kind == PropKind::FoamCover) tile->prop.growth_ticks=600;
    if (kind == PropKind::TensionSpring) {tile->prop.variant &= 3U;tile->prop.growth_ticks=18;}
    if (kind == PropKind::Grate) tile->prop.variant &= 1U;
    if (kind == PropKind::Barricade) {tile->prop.variant &= 3U;tile->prop.hp=static_cast<std::uint8_t>(prop_max_health(tile->prop));}
    if (kind == PropKind::Stove) {
        tile->prop.variant = 1; tile->prop.growth_ticks = 3600;
    }
    if (kind == PropKind::Candle) {
        tile->prop.variant = candle_lit_bit;
        tile->prop.growth_ticks = candle_fuel_ticks;
    }
    return true;
}

bool hit_prop(Game& game, Cell cell, int damage, Cell source) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr || damage <= 0 || tile->prop.kind == PropKind::None ||
        tile->prop.broken) return false;
    if (tile->prop.kind == PropKind::SpiderStrand) return cut_spider_strand(game,cell);
    Prop& prop = tile->prop;
    if (prop.kind==PropKind::LightTower) return hit_light_tower(game,cell,damage);
    if (prop.kind==PropKind::TallTree) return hit_tall_tree(game,cell,damage,source);
    if (prop.kind==PropKind::IcePillar) return hit_ice_pillar(game,cell,damage,source);
    if (prop.kind==PropKind::StreetLamp) return hit_streetlamp(game,cell,damage,source);
    if (prop.kind==PropKind::PayCage) alarm_pay_clerks(game,cell,source);
    if (prop.kind==PropKind::Conveyor) hit_belt_brake(game,cell,damage);
    prop.hp = static_cast<std::uint8_t>(std::max(0, static_cast<int>(prop.hp) - damage));
    if (prop.hp == 0) break_prop(game, cell, source, prop);
    else if (prop.kind == PropKind::Crate) {
        emit_sound(game,(game.tick+prop.hp)%2 ? SoundId::CrateKnock1 : SoundId::CrateKnock2,cell);
        if (game.impact_count<static_cast<int>(game.impacts.size()))
            game.impacts[static_cast<std::size_t>(game.impact_count++)]=
                {cell,source,Sprite::Crate,damage,false,PropKind::Crate};
    }
    else if (prop.kind==PropKind::FallenLog || prop.kind==PropKind::LogBridge) {
        emit_sound(game,SoundId::TreeChop,cell);
        if (game.impact_count<static_cast<int>(game.impacts.size()))
            game.impacts[static_cast<std::size_t>(game.impact_count++)]=
                {cell,source,Sprite::FallenLog,damage,false,prop.kind};
    }
    else if (prop.kind==PropKind::Pallet || prop.kind==PropKind::PalletStack) emit_sound(game,SoundId::CrateKnock1,cell);
    else if (prop.kind==PropKind::BoundRocks) emit_sound(game,SoundId::OreHit,cell);
    else if (prop.kind==PropKind::ContainerSide) emit_sound(game,SoundId::GrateHit,cell);
    else if (prop.kind == PropKind::SteamDrive) emit_sound(game,SoundId::BeltHit,cell);
    else if (prop.kind == PropKind::WaterPipe) emit_sound(game,SoundId::PipeHit,cell);
    else if (prop.kind == PropKind::SnowPile) emit_sound(game,SoundId::SnowScrape,cell);
    else if (prop.kind == PropKind::IceRubble) emit_sound(game,SoundId::IceBlockHit,cell);
    else if (prop.kind == PropKind::ChapelUrn) emit_sound(game,SoundId::PotBreak,cell);
    else if (prop.kind == PropKind::TowerWreck || prop.kind == PropKind::PoleWreck) emit_sound(game,SoundId::PoleHit,cell);
    else if (prop.kind == PropKind::FoamCover) emit_sound(game,SoundId::FoamTear,cell);
    else if (prop.kind == PropKind::PayCage) emit_sound(game,SoundId::PayRattle,cell);
    else if (prop.kind == PropKind::Conveyor) emit_sound(game,SoundId::BeltHit,cell);
    else if (prop.kind == PropKind::Grate || prop.kind==PropKind::Barricade) emit_sound(game,SoundId::GrateHit,cell);
    else if (prop.kind == PropKind::ScrapBin) emit_sound(game,SoundId::ScrapHit,cell);
    else if (prop.kind == PropKind::OreBin) emit_sound(game,SoundId::OreHit,cell);
    else if (prop.kind == PropKind::Doorstop) emit_sound(game,SoundId::WedgeHit,cell);
    else if (prop.kind == PropKind::SnowWindbreak) emit_sound(game,SoundId::ShelterHit,cell);
    else if (prop.kind == PropKind::CopperWire) emit_sound(game,SoundId::WireCut,cell);
    else if (prop.kind == PropKind::GroundingSpike) emit_sound(game,SoundId::SpikePlant,cell);
    else if (prop.kind == PropKind::MaintenanceLocker) emit_sound(game,SoundId::StoveHit,cell);
    else if (prop.kind == PropKind::FrozenLunchTin) emit_sound(game, SoundId::TinHit, cell);
    else if (prop.kind == PropKind::FishingCreel) emit_sound(game, SoundId::CreelHit, cell);
    else if (prop.kind == PropKind::WeatherVane) emit_sound(game, SoundId::OpticHit, cell);
    else if (prop.kind == PropKind::AlarmClock) emit_sound(game, SoundId::OpticHit, cell);
    else if (prop.kind != PropKind::BirdSeed && prop.kind != PropKind::Thorns)
        emit_sound(game, (prop.kind == PropKind::CrystalGrowth || optical_prop(prop) || prop.kind == PropKind::BeamLamp) ? SoundId::OpticHit : prop.kind == PropKind::SnowCache ? SoundId::SnowScrape : prop.kind == PropKind::IceBlock ? SoundId::IceBlockHit : prop.kind == PropKind::StrawDecoy ? SoundId::DecoyHit : SoundId::WoodCrack, cell);
    return true;
}

void step_on_prop(Game& game, int actor_slot) {
    if (trigger_tension_spring(game,actor_slot)) return;
    enter_spider_strand(game,actor_slot);
    const Entity& actor = game.entities[static_cast<std::size_t>(actor_slot)];
    Tile* tile = game.stage.at(actor.cell);
    if (tile && tile->prop.kind==PropKind::IceSpikes && !tile->prop.broken &&
        actor.health>0 && actor.toss.ticks==0 && wading_actor(actor)) {
        const Cell cell=actor.cell;
        break_prop(game,cell,cell,tile->prop);
        if (actor.vitals.traction==0 && actor.vitals.floor_insulation==0) {
            damage_entity(game,actor_slot,1,cell,false);
            apply_bleeding(game.entities[static_cast<std::size_t>(actor_slot)],60);
        }
        return;
    }
    if (tile != nullptr && tile->prop.kind == PropKind::Thorns && !tile->prop.broken) {
        const int damage = tile->prop.variant == 0 ? 6 : tile->prop.variant;
        const Cell cell = actor.cell;
        hit_prop(game, cell, 1, cell);
        damage_entity(game, actor_slot, damage, cell, false);
        emit_sound(game, SoundId::ThornPrick, cell);
        return;
    }
    if (tile != nullptr && !tile->prop.broken &&
        prop_spec(tile->prop.kind).breaks_on_step)
        break_prop(game, actor.cell, actor.cell, tile->prop);
}
