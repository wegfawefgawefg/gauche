#include "catalog.hpp"

PropSpec prop_spec(PropKind kind) {
    switch (kind) {
    case PropKind::MirrorShard: return {Sprite::MirrorShard, SoundId::OpticBreak, 8, false, false};
    case PropKind::CrystalLens: return {Sprite::CrystalLens, SoundId::OpticBreak, 18, false, false};
    case PropKind::SnowCache: return {Sprite::SnowCache, SoundId::SnowScrape, 4, false, true};
    case PropKind::IceBlock: return {Sprite::IceBlock, SoundId::IceBlockBreak, 35, true, false};
    case PropKind::StrawDecoy: return {Sprite::StrawDecoy, SoundId::DecoyTear, 40, true, false};
    case PropKind::Scarecrow: return {Sprite::Scarecrow, SoundId::ScarecrowTear, 28, true, false};
    case PropKind::BirdSeed: return {Sprite::BirdSeedPile, SoundId::SeedScatter, 12, false, false};
    case PropKind::Thorns: return {Sprite::ThornPatch, SoundId::ThornScatter, 3, false, false};
    case PropKind::Shoot: return {Sprite::Shoot, SoundId::PlantCut, 8, false, false};
    case PropKind::RootCover: return {Sprite::RootCover, SoundId::WoodCrack, 40, true, false};
    case PropKind::LanternPlant: return {Sprite::LanternPlant, SoundId::PlantCut, 12, false, false,
        {5, 800, {210, 240, 115}}};
    case PropKind::Leaves: return {Sprite::Leaves, SoundId::LeavesCrunch, 1, false, true};
    case PropKind::Twigs: return {Sprite::Twigs, SoundId::TwigSnap, 1, false, true};
    case PropKind::Fern: return {Sprite::Fern, SoundId::PlantCut, 3, false, true};
    case PropKind::TallGrass: return {Sprite::TallGrass, SoundId::PlantCut, 2, false, true};
    case PropKind::Puffball: return {Sprite::Puffball, SoundId::SporePuff, 1, false, true};
    case PropKind::RottenLog: return {Sprite::RottenLog, SoundId::WoodCrack, 22, true, false};
    case PropKind::Crate: return {Sprite::Crate, SoundId::WoodCrack, 18, true, false};
    case PropKind::Nest: return {Sprite::Nest, SoundId::TwigSnap, 3, false, true};
    case PropKind::ClayPot: return {Sprite::ClayPot, SoundId::PotBreak, 5, true, false};
    default: return {Sprite::Grass, SoundId::PlantCut, 0, false, false};
    }
}

bool prop_blocks(const Prop& prop) {
    return prop.kind != PropKind::None && !prop.broken && prop_spec(prop.kind).blocking;
}
