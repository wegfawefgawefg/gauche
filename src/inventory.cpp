#include "game.hpp"
#include "item_attribute.hpp"

#include <algorithm>

Item* Inventory::held() { return &slots[static_cast<std::size_t>(selected)]; }
const Item* Inventory::held() const { return &slots[static_cast<std::size_t>(selected)]; }

Item make_item(ItemKind kind, int count, ItemAttribute attribute) {
    Item item;
    item.kind = kind;
    item.attribute = item_accepts_attribute(kind, attribute) ?
        attribute : ItemAttribute::None;
    item.count = count;
    item.max_count = kind == ItemKind::None ? 0 :
        kind == ItemKind::Wall ? 99 :
        kind == ItemKind::Medkit || kind == ItemKind::Bandage ? 10 :
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
    case ItemKind::Bow: item.loaded = 1; item.spare = 20; break;
    case ItemKind::RocketLauncher: item.loaded = 1; item.spare = 4; break;
    case ItemKind::Shotgun: item.loaded = 6; item.spare = 30; break;
    case ItemKind::SMG: item.loaded = 30; item.spare = 120; break;
    default: break;
    }
    if (item.attribute == ItemAttribute::Durable) {
        item.max_durability *= 2;
        item.max_uses *= 2;
    } else if (item.attribute == ItemAttribute::Fragile)
        item.max_uses = std::max(1, item.max_uses / 2);
    item.durability = item.max_durability;
    item.uses = item.max_uses;
    return item;
}

Sprite item_sprite(ItemKind kind) {
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
    return item.kind == ItemKind::BearTrap && item.opened ?
        Sprite::BearTrapOpen : item_sprite(item.kind);
}

const char* item_name(ItemKind kind) {
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
    }
    return "Unknown";
}

bool insert_item(Inventory& inventory, Item item) {
    if (item.kind == ItemKind::None || item.count <= 0 || item.max_count <= 0) return false;
    const Inventory original = inventory;
    if (item.max_count > 1) {
        for (Item& slot : inventory.slots) {
            if (slot.kind != item.kind || slot.attribute != item.attribute ||
                slot.opened != item.opened || slot.max_count != item.max_count ||
                slot.consume_on_use != item.consume_on_use ||
                slot.count >= slot.max_count) continue;
            const int transfer = std::min(slot.max_count - slot.count, item.count);
            slot.count += transfer;
            item.count -= transfer;
            if (item.count == 0) return true;
        }
    }
    for (Item& slot : inventory.slots) {
        if (slot.kind == ItemKind::None) {
            slot = item;
            slot.count = std::min(item.count, item.max_count);
            item.count -= slot.count;
            if (item.count == 0) return true;
        }
    }
    inventory = original;
    return false;
}
