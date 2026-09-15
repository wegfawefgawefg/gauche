#include "catalog.hpp"
#include "candle.hpp"

PropSpec prop_spec(PropKind kind) {
    switch (kind) {
    case PropKind::Doorstop: return {Sprite::EmergencyDoorstop,SoundId::WedgeBreak,25,false,false};
    case PropKind::CopperWire: return {Sprite::WireJunction,SoundId::WireCut,4,false,false};
    case PropKind::GroundingSpike: return {Sprite::GroundingSpike,SoundId::SpikeBreak,8,false,false};
    case PropKind::MaintenanceLocker: return {Sprite::MaintenanceLocker,SoundId::LockerOpen,24,true,false};
    case PropKind::SpiderStrand: return {Sprite::SpiderStrand,SoundId::SpiderCut,1,false,false};
    case PropKind::Stove: return {Sprite::Stove,SoundId::StoveBreak,40,true,false,{4,950,{255,168,78}}};
    case PropKind::CandleCabinet: return {Sprite::CandleCabinet,SoundId::CabinetOpen,18,true,false};
    case PropKind::Candle: return {Sprite::CandleStub, SoundId::CandleBreak, 6, false, false, {3,650,{255,182,82}}};
    case PropKind::FrozenLunchTin: return {Sprite::FrozenLunchTin, SoundId::TinOpen, 16, true, false};
    case PropKind::WeatherVane: return {Sprite::WeatherVane, SoundId::VaneBreak, 18, false, false};
    case PropKind::FishingCreel: return {Sprite::FishingCreel, SoundId::CreelBreak, 12, true, false};
    case PropKind::AlarmClock: return {Sprite::AlarmClock, SoundId::ClockBreak, 8, false, false};
    case PropKind::LensCase: return {Sprite::LensCase, SoundId::LensCaseBreak, 16, true, false};
    case PropKind::BeamLamp: return {Sprite::BeamLamp, SoundId::LampBreak, 24, false, false, {5, 850, {179, 208, 233}}};
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

int prop_max_health(const Prop& prop) {
    const int base = prop_spec(prop.kind).health;
    if (prop.kind == PropKind::Doorstop && prop.variant == 1) return base * 2;
    if (prop.kind == PropKind::Candle && (prop.variant & candle_durable_bit)) return base * 2;
    return prop.kind == PropKind::AlarmClock && prop.variant == 1 ? base * 2 : base;
}
