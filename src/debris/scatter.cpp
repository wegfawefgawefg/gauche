#include "system.hpp"

namespace {

std::uint32_t roll(std::uint64_t& seed) {
    seed ^= seed >> 12; seed ^= seed << 25; seed ^= seed >> 27;
    return static_cast<std::uint32_t>((seed * 2685821657736338717ULL) >> 32);
}

float unit(std::uint64_t& seed) { return static_cast<float>(roll(seed) % 1024) / 1024.0F; }

} // namespace

void scatter_material(LooseDebris& debris, Cell cell, DebrisKind kind,
                       int count, std::uint64_t seed, bool settled) {
    if (seed == 0) seed = 1;
    for (int i = 0; i < count; ++i) {
        LoosePiece p;
        p.kind = kind;
        p.x = static_cast<float>(cell.x) + .15F + unit(seed) * .7F;
        p.y = static_cast<float>(cell.y) + .15F + unit(seed) * .7F;
        p.vx = settled ? 0 : (unit(seed) - .5F) * .17F;
        p.vy = settled ? 0 : (unit(seed) - .5F) * .17F;
        p.angle = unit(seed) * 360;
        p.spin = settled ? 0 : (unit(seed) - .5F) * 12;
        p.settled = settled ? 30 : 0;
        add_loose_piece(debris, p);
    }
}

void scatter_prop_debris(LooseDebris& debris, Cell cell, PropKind kind,
                         std::uint64_t seed, bool settled) {
    if (kind==PropKind::FoamCover) return; // Collapses in place, not into wooden fragments.
    DebrisKind first = DebrisKind::WoodChip, second = DebrisKind::Bark;
    switch (kind) {
    case PropKind::ChapelAltar: first=DebrisKind::WoodChip;second=DebrisKind::Cloth;break;
    case PropKind::ChapelRunner: first=second=DebrisKind::Cloth;break;
    case PropKind::ChapelUrn: first=second=DebrisKind::Pottery;break;
    case PropKind::ChapelWax: first=DebrisKind::Wax;second=DebrisKind::CharredWick;break;
    case PropKind::LightTower: case PropKind::StreetLamp: first=DebrisKind::MirrorChip;second=DebrisKind::CopperCurl;break;
    case PropKind::TowerWreck: case PropKind::PoleWreck: first=DebrisKind::TinCurl;second=DebrisKind::SteelWasher;break;
    case PropKind::Conveyor: first=DebrisKind::BeltRubber; second=DebrisKind::SteelWasher; break;
    case PropKind::Barricade: case PropKind::Grate: first = DebrisKind::SteelWasher; second = DebrisKind::TinCurl; break;
    case PropKind::ScrapBin: first = DebrisKind::TinCurl; second = DebrisKind::CopperCurl; break;
    case PropKind::OreBin: first = DebrisKind::OreFlake; second = DebrisKind::BasaltChip; break;
    case PropKind::CrystalGrowth: first = second = DebrisKind::CrystalSplinter; break;
    case PropKind::TallTree: case PropKind::FallenLog: case PropKind::LogBridge:
    case PropKind::BridgePlank: first = DebrisKind::WoodChip; second = DebrisKind::RopeFiber; break;
    case PropKind::Doorstop: first = DebrisKind::BrassCase; second = DebrisKind::BrassRivet; break;
    case PropKind::HoistWreck: first=DebrisKind::ChainLink;second=DebrisKind::BasaltChip;break;
    case PropKind::PayCage: first=DebrisKind::SteelWasher;second=DebrisKind::BentNail;break;
    case PropKind::TensionSpring: first=second=DebrisKind::SteelWasher; break;
    case PropKind::CopperWire: first = second = DebrisKind::CopperCurl; break;
    case PropKind::GroundingSpike: first = DebrisKind::CopperCurl; second = DebrisKind::BrassRivet; break;
    case PropKind::MaintenanceLocker: first = DebrisKind::BrassRivet; second = DebrisKind::CopperCurl; break;
    case PropKind::Stove: first = DebrisKind::CoalCrumb; second = DebrisKind::CopperCurl; break;
    case PropKind::Candle: first = DebrisKind::Wax; second = DebrisKind::CharredWick; break;
    case PropKind::FrozenLunchTin: first = DebrisKind::TinLid; second = DebrisKind::IceChip; break;
    case PropKind::WeatherVane: first = DebrisKind::CopperCurl; second = DebrisKind::BrassCase; break;
    case PropKind::FishingCreel: first = DebrisKind::Wicker; second = DebrisKind::RopeFiber; break;
    case PropKind::AlarmClock: first = DebrisKind::ClockGear; second = DebrisKind::BrassCase; break;
    case PropKind::LensCase: first = DebrisKind::WoodChip; second = DebrisKind::CrystalSplinter; break;
    case PropKind::BeamLamp: first = DebrisKind::MirrorChip; second = DebrisKind::BrassCase; break;
    case PropKind::MirrorShard: first = second = DebrisKind::MirrorChip; break;
    case PropKind::CrystalLens: first = second = DebrisKind::CrystalSplinter; break;
    case PropKind::SnowWindbreak: case PropKind::SnowCache: first = second = DebrisKind::SnowClump; break;
    case PropKind::IcePillar: case PropKind::IceRubble: case PropKind::IceBlock: first = second = DebrisKind::IceChip; break;
    case PropKind::StrawDecoy: case PropKind::Scarecrow: first = DebrisKind::Straw; second = DebrisKind::Cloth; break;
    case PropKind::BirdSeed: first = second = DebrisKind::SeedHusk; break;
    case PropKind::Thorns: first = DebrisKind::Twig; second = DebrisKind::PineNeedle; break;
    case PropKind::Shoot: case PropKind::RootCover: case PropKind::LanternPlant:
        first = DebrisKind::Root; second = DebrisKind::FernLeaf; break;
    case PropKind::Leaves: first = DebrisKind::OakLeaf; second = DebrisKind::BirchLeaf; break;
    case PropKind::Twigs: first = DebrisKind::Twig; second = DebrisKind::PineNeedle; break;
    case PropKind::Fern: first = DebrisKind::FernLeaf; second = DebrisKind::Root; break;
    case PropKind::TallGrass: first = DebrisKind::GrassBlade; second = DebrisKind::SeedHusk; break;
    case PropKind::Puffball: first = DebrisKind::MushroomCap; second = DebrisKind::Spore; break;
    case PropKind::Nest: first = DebrisKind::Twig; second = DebrisKind::Feather; break;
    case PropKind::ClayPot: first = second = DebrisKind::Pottery; break;
    default: break;
    }
    scatter_material(debris, cell, first, kind == PropKind::FrozenLunchTin ? 1 : settled ? 2 : 5, seed, settled);
    scatter_material(debris, cell, second, settled ? 1 : 3, seed ^ 0x529ec831ULL, settled);
    if (kind == PropKind::Crate)
        scatter_material(debris, cell, DebrisKind::BentNail, settled ? 1 : 2, seed + 71, settled);
    if (kind == PropKind::Puffball)
        scatter_material(debris, cell, DebrisKind::MushroomStem, 1, seed + 17, settled);
    if (kind == PropKind::Nest)
        scatter_material(debris, cell, DebrisKind::Acorn, 1, seed + 31, settled);
    if (kind == PropKind::FishingCreel)
        scatter_material(debris, cell, DebrisKind::FishingFloat, 1, seed + 63, settled);
    if (kind == PropKind::LensCase)
        scatter_material(debris, cell, DebrisKind::Felt, 2, seed + 59, settled);
    if (kind == PropKind::Crate || kind == PropKind::CandleCabinet)
        scatter_material(debris, cell, DebrisKind::Cloth, 1, seed + 47, settled);
}

void prepare_debris(LooseDebris& debris, const Stage& stage) {
    if (debris.ready) return;
    debris.ready = true;
    debris.pieces.reserve(4096);
    rebuild_debris_index(debris);
    // REJOIN: Broken props retain enough state to reconstruct quiet, settled litter.
    for (int y = 0; y < stage.height; ++y)
        for (int x = 0; x < stage.width; ++x) {
            const Prop& prop = stage.at({x, y})->prop;
            if (prop.kind != PropKind::None && prop.broken)
                scatter_prop_debris(debris, {x, y}, prop.kind,
                    static_cast<std::uint64_t>(y * stage.width + x + 1), true);
        }
}
