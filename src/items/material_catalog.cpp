#include "materials.hpp"

namespace {
constexpr RegionalItem torch{"Torch", "A portable flame. Strike and ignite dry growth, oil or sap. 90 swings.",
    Sprite::Torch, {1, 1, 0, 6, 35, PatternEffect::Damage},
    ItemAction::Melee, 8, 1, false, 90, 0, 0, 0, 0, SoundId::TorchSwing};
constexpr RegionalItem lighter{"Lighter", "A tiny spark, 20 uses. Try oil, sap, or dry growth.",
    Sprite::Lighter, {1, 1, 0, 0, 25, PatternEffect::Utility},
    ItemAction::Material, 6, 1, false, 20, 0, 0, 0, 0, SoundId::LighterSpark};
constexpr RegionalItem oil{"Oil flask", "Oily ground slips a walker one extra cell. A spark turns it into spreading fire.",
    Sprite::OilFlask, {1, 5, 2, 0, 40, PatternEffect::Utility},
    ItemAction::Throw, 8, 3, true, 0, 0, 0, 0, 0, SoundId::BottleThrow};
constexpr RegionalItem sap{"Sap jar", "Throw a sticky cross. Slows steps and burns longer than oil.",
    Sprite::SapJar, {1, 4, 1, 0, 45, PatternEffect::Utility},
    ItemAction::Throw, 9, 3, true, 0, 0, 0, 0, 0, SoundId::BottleThrow};
constexpr RegionalItem water{"Water flask", "Pour three facing cells. Quench flames, clear spores and cool lava.",
    Sprite::WaterFlask, {1, 1, 0, 0, 30, PatternEffect::Utility, false, 1},
    ItemAction::Material, 5, 5, true, 0, 0, 0, 0, 0, SoundId::WaterPour};
constexpr RegionalItem spores{"Mushroom spores", "Throw a lingering sleep cloud. It catches allies and enemies alike.",
    Sprite::MushroomSpores, {1, 3, 1, 0, 45, PatternEffect::Utility},
    ItemAction::Throw, 10, 4, true, 0, 0, 0, 0, 0, SoundId::BottleThrow};
constexpr RegionalItem smoke{"Smoke pot", "Throw a concealing cloud. Creatures lose sight; attacks still pass through.",
    Sprite::SmokePot, {1, 5, 2, 0, 45, PatternEffect::Utility},
    ItemAction::Throw, 10, 3, true, 0, 0, 0, 0, 0, SoundId::BottleThrow};
constexpr RegionalItem honey{"Honey pot", "Place a sweet bear lure on a sticky patch. Don't stand beside the diner.",
    Sprite::HoneyPot, {1, 1, 0, 0, 35, PatternEffect::Utility},
    ItemAction::Material, 8, 3, true, 0, 0, 0, 0, 0, SoundId::HoneyPlace};
}

const RegionalItem* forest_material_item(ItemKind kind) {
    switch (kind) {
    case ItemKind::Torch: return &torch;
    case ItemKind::Lighter: return &lighter;
    case ItemKind::OilFlask: return &oil;
    case ItemKind::SapJar: return &sap;
    case ItemKind::WaterFlask: return &water;
    case ItemKind::MushroomSpores: return &spores;
    case ItemKind::SmokePot: return &smoke;
    case ItemKind::HoneyPot: return &honey;
    default: return nullptr;
    }
}
