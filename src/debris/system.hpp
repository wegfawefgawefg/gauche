#pragma once

#include "../game.hpp"
#include "../view.hpp"
#include "../lighting/field.hpp"

#include <array>
#include <vector>

enum class DebrisKind : std::uint8_t {
    OakLeaf, BirchLeaf, PineNeedle, Twig, Bark, WoodChip, Root, FernLeaf, GrassBlade,
    MushroomCap, MushroomStem, Spore, Acorn, SeedHusk, Feather, BoneChip,
    Pottery, Cloth, BrassCase, StoneChip, Straw, IceChip, SnowClump, MirrorChip, CrystalSplinter, Felt, ClockGear, WoolTuft, RopeFiber, FishBone, Wicker, FishingFloat, GlobeGlass, CopperCurl, KelpScrap, TinLid, Wax, CharredWick, CoalCrumb, BrassRivet, FlareCinder, BasaltChip, OreFlake, SteelWasher, TinCurl, RivetCasing, BeltRubber, BentNail, ChainLink, Count
};

struct LoosePiece {
    DebrisKind kind = DebrisKind::OakLeaf;
    float x = 0, y = 0, vx = 0, vy = 0;
    float angle = 0, spin = 0;
    int next = -1;
    std::uint8_t settled = 0;
    std::uint8_t count = 1;
};

struct LooseDebris {
    std::vector<LoosePiece> pieces;
    std::array<int, 1024> buckets{};
    bool ready = false;
    std::size_t recycle = 0;
    LooseDebris() { buckets.fill(-1); }
};

void rebuild_debris_index(LooseDebris& debris);
void add_loose_piece(LooseDebris& debris, LoosePiece piece);
void prepare_debris(LooseDebris& debris, const Stage& stage);
void scatter_prop_debris(LooseDebris& debris, Cell cell, PropKind kind,
                         std::uint64_t seed, bool settled = false);
void scatter_material(LooseDebris& debris, Cell cell, DebrisKind kind,
                       int count, std::uint64_t seed, bool settled = false);
void push_debris(LooseDebris& debris, Cell cell, float radius, float force,
                  Cell direction = {});
void step_debris(LooseDebris& debris, const Stage& stage, std::uint64_t tick, bool wind);
void draw_debris(SDL_Renderer* renderer, const GameGraphics& graphics,
                  const LooseDebris& debris, ViewCamera camera, float zoom,
                  const LightingCache& lighting);

float debris_friction(DebrisKind kind);
void rake_debris(LooseDebris& debris, const Stage& stage, const SweepEvent& sweep);
