#include "item_pattern.hpp"

#include <algorithm>
#include <cstdlib>

ItemPattern item_pattern(ItemKind kind) {
    switch (kind) {
    case ItemKind::Fist: return {1, 1, 0, 10, 12, PatternEffect::Damage};
    case ItemKind::Stick: return {1, 1, 0, 17, 16, PatternEffect::Damage};
    case ItemKind::Pickaxe: return {1, 1, 0, 22, 24, PatternEffect::Damage};
    case ItemKind::Pistol: return {1, 9, 0, 16, 12, PatternEffect::Damage, true};
    case ItemKind::Shotgun: return {1, 5, 0, 48, 32, PatternEffect::Damage, true};
    case ItemKind::SMG: return {1, 8, 0, 9, 4, PatternEffect::Damage, true};
    case ItemKind::Musket: case ItemKind::Bow:
        return {1, 14, 0, 35, 40, PatternEffect::Damage, true};
    case ItemKind::RocketLauncher:
        return {1, 14, 2, 80, 48, PatternEffect::Damage, true};
    case ItemKind::Bomb: return {0, 3, 2, 65, 45, PatternEffect::Damage};
    case ItemKind::Wall: return {1, 2, 0, 0, 6, PatternEffect::Utility};
    case ItemKind::Buckler: return {1, 1, 0, 0, 30, PatternEffect::Utility};
    case ItemKind::BearTrap: case ItemKind::Mine:
        return {1, 1, 0, 0, 20, PatternEffect::Utility};
    case ItemKind::SleepMeds: return {0, 3, 0, 0, 30, PatternEffect::Utility};
    case ItemKind::Medkit: return {0, 0, 0, 0, 300, PatternEffect::Heal};
    case ItemKind::Bandage: return {0, 0, 0, 0, 120, PatternEffect::Heal};
    case ItemKind::Bandaid: return {0, 0, 0, 0, 12, PatternEffect::Heal};
    case ItemKind::RawMeat: return {0, 0, 0, 0, 12, PatternEffect::Heal};
    case ItemKind::CookedMeat: return {0, 0, 0, 0, 90, PatternEffect::Heal};
    case ItemKind::Ammo: case ItemKind::ConductorHat:
        return {0, 0, 0, 0, 0, PatternEffect::Utility};
    case ItemKind::None: break;
    }
    return {};
}

Cell aimed_item_target(const Entity& user, Cell aim, ItemPattern pattern) {
    const Cell direction = user.facing;
    const int reach = pattern.ray ? 1 :
        std::clamp(std::max(std::abs(aim.x), std::abs(aim.y)),
                   std::max(1, pattern.minimum), std::max(1, pattern.maximum));
    return {user.cell.x + direction.x * reach,
            user.cell.y + direction.y * reach};
}
