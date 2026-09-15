#include "game.hpp"
#include "props/candle.hpp"
#include "items/catalog.hpp"
#include "items/storm_lantern.hpp"
#include "item_attribute.hpp"

#include <algorithm>

namespace {

LightEmitter initial_item_light(ItemKind kind) {
    switch (kind) {
    case ItemKind::Medkit: return {3, 420, {71, 255, 92}};
    case ItemKind::ConductorHat: return {3, 460, {255, 184, 69}};
    case ItemKind::RocketLauncher: case ItemKind::Mine:
        return {3, 350, {255, 94, 51}};
    default: return {};
    }
}

} // namespace

Item* Inventory::held() { return &slots[static_cast<std::size_t>(selected)]; }
const Item* Inventory::held() const { return &slots[static_cast<std::size_t>(selected)]; }

Item make_item(ItemKind kind, int count, ItemAttribute attribute) {
    Item item;
    item.kind = kind;
    item.light = initial_item_light(kind);
    item.attribute = item_accepts_attribute(kind, attribute) ?
        attribute : ItemAttribute::None;
    item.count = count;
    item.max_count = kind == ItemKind::None ? 0 :
        kind == ItemKind::Wall ? 99 :
        kind == ItemKind::Medkit || kind == ItemKind::Bandage || kind == ItemKind::Ammo ? 10 :
        kind == ItemKind::Bandaid || kind == ItemKind::Bomb ||
        kind == ItemKind::SleepMeds || kind == ItemKind::BearTrap ||
        kind == ItemKind::Mine || kind == ItemKind::RawMeat ||
        kind == ItemKind::CookedMeat ? 20 : 1;
    item.consume_on_use = kind == ItemKind::Wall || kind == ItemKind::Medkit ||
        kind == ItemKind::Bandage || kind == ItemKind::Bandaid ||
        kind == ItemKind::ConductorHat || kind == ItemKind::Ammo ||
        kind == ItemKind::Bomb || kind == ItemKind::SleepMeds ||
        kind == ItemKind::BearTrap || kind == ItemKind::Mine ||
        kind == ItemKind::RawMeat || kind == ItemKind::CookedMeat;
    switch (kind) {
    case ItemKind::Buckler: item.max_durability = 30; break;
    case ItemKind::Stick: item.max_uses = 18; break;
    case ItemKind::Pickaxe: item.max_uses = 32; break;
    case ItemKind::Pistol: item.loaded = 12; item.spare = 48; break;
    case ItemKind::Musket: item.loaded = 1; item.spare = 12; break;
    case ItemKind::Bow: item.loaded = 20; item.spare = 0; break;
    case ItemKind::RocketLauncher: item.loaded = 1; item.spare = 4; break;
    case ItemKind::Shotgun: item.loaded = 6; item.spare = 30; break;
    case ItemKind::SMG: item.loaded = 30; item.spare = 120; break;
    default: break;
    }
    if (const RegionalItem* spec = regional_item(kind)) {
        item.max_count = spec->max_count;
        item.consume_on_use = spec->consume;
        item.max_uses = spec->uses;
        item.loaded = spec->magazine;
        item.spare = spec->spare;
        item.dig_power = spec->dig_power;
        item.max_durability = spec->durability;
        item.light = spec->light;
    }
    if (item.attribute == ItemAttribute::Durable) {
        item.max_durability *= 2;
        item.max_uses *= 2;
    } else if (item.attribute == ItemAttribute::Fragile) {
        if (item.max_uses > 0) item.max_uses = std::max(1, item.max_uses / 2);
        if (item.max_durability > 0) item.max_durability = std::max(1, item.max_durability / 2);
    }
    if (kind == ItemKind::StormLantern) { item.loaded = lantern_fuel_ticks; item.opened = true; }
    if (kind == ItemKind::CandleStub) item.loaded = candle_fuel_ticks;
    if (kind == ItemKind::Pickaxe) item.dig_power = 2;
    item.durability = item.max_durability;
    item.uses = item.max_uses;
    return item;
}

Sprite item_sprite(ItemKind kind) {
    if (const RegionalItem* spec = regional_item(kind)) return spec->sprite;
    switch (kind) {
    case ItemKind::Wall: return Sprite::Wall;
    case ItemKind::Medkit: return Sprite::Medkit;
    case ItemKind::Bandage: return Sprite::Bandage;
    case ItemKind::Bandaid: return Sprite::Bandaid;
    case ItemKind::ConductorHat: return Sprite::ConductorHat;
    case ItemKind::Buckler: return Sprite::Buckler;
    case ItemKind::Pistol: return Sprite::Pistol;
    case ItemKind::Musket: return Sprite::Musket;
    case ItemKind::Bow: return Sprite::Bow;
    case ItemKind::RocketLauncher: return Sprite::RocketLauncher;
    case ItemKind::Ammo: return Sprite::Ammo;
    case ItemKind::Bomb: return Sprite::Bomb;
    case ItemKind::SleepMeds: return Sprite::SleepMeds;
    case ItemKind::Stick: return Sprite::Stick;
    case ItemKind::Shotgun: return Sprite::Shotgun;
    case ItemKind::SMG: return Sprite::SMG;
    case ItemKind::BearTrap: return Sprite::BearTrap;
    case ItemKind::Mine: return Sprite::Mine;
    case ItemKind::Pickaxe: return Sprite::Pickaxe;
    case ItemKind::RawMeat: return Sprite::RawMeat;
    case ItemKind::CookedMeat: return Sprite::CookedMeat;
    default: return Sprite::Fist;
    }
}

Sprite item_sprite(const Item& item) {
    if (item.kind == ItemKind::EchoPebble) return item.loaded>0 ? Sprite::EchoPebbleReady : Sprite::EchoPebble;
    if (item.kind == ItemKind::HeatSiphon) return item.loaded>0 ? Sprite::SiphonCharged : Sprite::HeatSiphon;
    if (item.kind == ItemKind::StormLantern) return lantern_sprite(item);
    if (item.kind == ItemKind::SteamKettle) return item.loaded == 2 ? Sprite::KettleHot :
        item.loaded == 1 ? Sprite::KettleFull : Sprite::SteamKettle;
    return item.kind == ItemKind::BearTrap && item.opened ?
        Sprite::BearTrapOpen : item_sprite(item.kind);
}

const char* item_name(ItemKind kind) {
    if (const RegionalItem* spec = regional_item(kind)) return spec->name;
    switch (kind) {
    case ItemKind::None: return "Empty";
    case ItemKind::Wall: return "Wall";
    case ItemKind::Medkit: return "Medkit";
    case ItemKind::Bandage: return "Bandage";
    case ItemKind::Bandaid: return "Bandaid";
    case ItemKind::Fist: return "Fist";
    case ItemKind::ConductorHat: return "Conductor Hat";
    case ItemKind::Buckler: return "Buckler";
    case ItemKind::Pistol: return "Pistol";
    case ItemKind::Musket: return "Musket";
    case ItemKind::Bow: return "Bow";
    case ItemKind::RocketLauncher: return "Rocket Launcher";
    case ItemKind::Ammo: return "Ammo";
    case ItemKind::Bomb: return "Bomb";
    case ItemKind::SleepMeds: return "Sleep Meds";
    case ItemKind::Stick: return "Stick";
    case ItemKind::Shotgun: return "Shotgun";
    case ItemKind::SMG: return "SMG";
    case ItemKind::BearTrap: return "Bear Trap";
    case ItemKind::Mine: return "Mine";
    case ItemKind::Pickaxe: return "Pickaxe";
    case ItemKind::RawMeat: return "Raw Meat";
    case ItemKind::CookedMeat: return "Cooked Meat";
    default: break;
    }
    return "Unknown";
}
