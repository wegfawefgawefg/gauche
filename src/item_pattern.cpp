#include "items/heated_water.hpp"
#include "item_pattern.hpp"
#include "items/catalog.hpp"

#include <algorithm>
#include <cstdlib>

int pattern_half_width(ItemPattern pattern, int reach) {
    if (pattern.light_shape!=LightShape::Omni)
        return std::min(pattern.maximum-reach,(reach-1)/(pattern.light_shape==LightShape::Beam ? 2 : 1));
    return pattern.cone ? std::min(pattern.half_width, std::max(0, reach - 1)) : pattern.half_width;
}

ItemPattern item_pattern(ItemKind kind) {
    if (const RegionalItem* spec = regional_item(kind)) return spec->pattern;
    switch (kind) {
    case ItemKind::Fist: return {1, 1, 0, 4, 12, PatternEffect::Damage};
    case ItemKind::Stick: return {1, 1, 0, 17, 16, PatternEffect::Damage};
    case ItemKind::Pickaxe: return {1, 1, 0, 22, 24, PatternEffect::Damage};
    case ItemKind::Pistol: return {1, 9, 0, 16, 12, PatternEffect::Damage, true};
    case ItemKind::Shotgun: return {1, 5, 0, 48, 32, PatternEffect::Damage, true};
    case ItemKind::SMG: return {1, 8, 0, 9, 4, PatternEffect::Damage, true};
    case ItemKind::Bow: return {1, 14, 0, 35, 16, PatternEffect::Damage, true};
    case ItemKind::Musket:
        return {1, 14, 0, 35, 40, PatternEffect::Damage, true};
    case ItemKind::RocketLauncher:
        return {1, 14, 2, 80, 48, PatternEffect::Damage, true};
    case ItemKind::Bomb: return {3, 3, 2, 65, 45, PatternEffect::Damage};
    case ItemKind::Wall: return {1, 2, 0, 0, 6, PatternEffect::Utility};
    case ItemKind::Buckler: return {1, 1, 0, 0, 30, PatternEffect::Utility};
    case ItemKind::BearTrap: case ItemKind::Mine:
        return {1, 1, 0, 0, 20, PatternEffect::Utility};
    case ItemKind::SleepMeds: return {0, 3, 0, 0, 30, PatternEffect::Utility};
    case ItemKind::Medkit: return {0, 0, 0, 0, 300, PatternEffect::Heal, false, 0, 100};
    case ItemKind::Bandage: return {0, 0, 0, 0, 120, PatternEffect::Heal, false, 0, 10};
    case ItemKind::Bandaid: return {0, 0, 0, 0, 12, PatternEffect::Heal, false, 0, 1};
    case ItemKind::RawMeat: return {0, 0, 0, 0, 12, PatternEffect::Heal, false, 0, 4};
    case ItemKind::CookedMeat: return {0, 0, 0, 0, 90, PatternEffect::Heal, false, 0, 18};
    case ItemKind::Ammo: case ItemKind::ConductorHat:
        return {0, 0, 0, 0, 0, PatternEffect::Utility};
    default: break;
    }
    return {};
}

ItemPattern item_pattern(const Item& item) {
    ItemPattern pattern = item_pattern(item.kind);
    if (item.kind==ItemKind::LunchTin && item.loaded==0)
        pattern={1,5,0,2,30,PatternEffect::Damage,true};
    if (item.kind==ItemKind::StormLantern) {
        pattern.light_shape=item.light.shape;
        pattern.maximum=item.light.shape==LightShape::Beam ? 12 : 8;
        pattern.minimum=1; pattern.cone=true;
        for (int step=1;step<=pattern.maximum;++step)
            pattern.half_width=std::max(pattern.half_width,pattern_half_width(pattern,step));
    }
    switch (item.attribute) {
    case ItemAttribute::None: case ItemAttribute::Durable: break;
    case ItemAttribute::Strong:
        pattern.damage = (pattern.damage * 5 + 3) / 4;
        break;
    case ItemAttribute::Agile:
        pattern.cooldown = std::max(1, (pattern.cooldown * 3 + 2) / 4);
        break;
    case ItemAttribute::Fragile:
        pattern.damage = (pattern.damage * 7 + 4) / 5;
        break;
    case ItemAttribute::Heavy:
        pattern.damage = (pattern.damage * 4 + 2) / 3;
        pattern.cooldown = (pattern.cooldown * 3 + 1) / 2;
        break;
    case ItemAttribute::Big:
        if (item.kind == ItemKind::SkateBlade) ++pattern.half_width;
        else if (item.kind == ItemKind::HeatSiphon || item.kind == ItemKind::SteamKettle) {
            ++pattern.half_width; ++pattern.maximum;
        } else if (item.kind == ItemKind::HandBellows || item.kind == ItemKind::SnowScoop || item.kind == ItemKind::GritPouch || item.kind == ItemKind::ThornCaltrops || item.kind == ItemKind::ThrowingNet) ++pattern.half_width;
        else if (pattern.blast_radius > 0) ++pattern.blast_radius;
        else if (pattern.effect == PatternEffect::Damage && !pattern.ray) {
            pattern.maximum = std::max(2, pattern.maximum);
            pattern.half_width = 1;
        }
        break;
    case ItemAttribute::Long:
        pattern.maximum += pattern.ray ? 4 : 1;
        break;
    case ItemAttribute::Piercing:
        pattern.piercing = pattern.ray;
        break;
    case ItemAttribute::Restorative:
        pattern.heal = (pattern.heal * 3 + 1) / 2;
        pattern.cooldown = (pattern.cooldown * 5 + 3) / 4;
        break;
    }
    if (item.kind==ItemKind::HeatSiphon)
        pattern.damage=item.loaded>0 ? std::max(1,pattern.damage*std::min(item.loaded,300)/300) : 0;
    // CONTENTS: Empty water tools fill one cell; cold water keeps the hot footprint.
    if (heated_water_item(item.kind)) {
        if (item.loaded == 0) { pattern.minimum = pattern.maximum = 1; pattern.half_width = 0; }
        if (item.loaded != 2) { pattern.damage = 0; pattern.effect = PatternEffect::Utility; }
    }
    return pattern;
}

Cell aimed_item_target(const Entity& user, Cell aim, ItemPattern pattern) {
    const Cell direction = user.facing;
    const RegionalItem* spec = regional_item(user.inventory.held()->kind);
    const int reach = (user.inventory.held()->kind == ItemKind::Bomb ||
        (spec != nullptr && spec->action == ItemAction::Throw)) ? pattern.maximum :
        pattern.ray ? 1 :
        std::clamp(std::max(std::abs(aim.x), std::abs(aim.y)),
                   std::max(1, pattern.minimum), std::max(1, pattern.maximum));
    return {user.cell.x + direction.x * reach,
            user.cell.y + direction.y * reach};
}

ItemPattern active_item_pattern(const Item& item, const Entity& user) {
    ItemPattern pattern=item_pattern(item);
    if (user.vitals.slide_momentum==0) pattern.momentum_tip=0;
    return pattern;
}
