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
    DebrisKind first = DebrisKind::WoodChip, second = DebrisKind::Bark;
    switch (kind) {
    case PropKind::Scarecrow: first = DebrisKind::Straw; second = DebrisKind::Cloth; break;
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
    scatter_material(debris, cell, first, settled ? 2 : 5, seed, settled);
    scatter_material(debris, cell, second, settled ? 1 : 3, seed ^ 0x529ec831ULL, settled);
    if (kind == PropKind::Puffball)
        scatter_material(debris, cell, DebrisKind::MushroomStem, 1, seed + 17, settled);
    if (kind == PropKind::Nest)
        scatter_material(debris, cell, DebrisKind::Acorn, 1, seed + 31, settled);
    if (kind == PropKind::Crate)
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
