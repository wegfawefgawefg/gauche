#pragma once

#include "../graphics.hpp"
#include "../sound.hpp"
#include "../lighting/emitter.hpp"

#include <cstdint>

enum class PropKind : std::uint8_t { None, Leaves, Twigs, Fern, TallGrass,
    Puffball, RottenLog, Crate, Nest, ClayPot, Shoot, RootCover, LanternPlant, BirdSeed, Thorns, Scarecrow, StrawDecoy, IceBlock, SnowCache, MirrorShard, CrystalLens, BeamLamp, LensCase, AlarmClock, FishingCreel, WeatherVane, FrozenLunchTin, Candle, CandleCabinet, Stove, SpiderStrand, MaintenanceLocker, CopperWire, GroundingSpike, Doorstop, BridgePlank, CrystalGrowth, SnowWindbreak, Grate, ScrapBin, OreBin, Conveyor, Barricade, TensionSpring, PayCage, FoamCover, HoistWreck, RailPoints, StreetLamp, PoleWreck, ChapelPew, ChapelAltar, ChapelRunner, ChapelUrn, ChapelPewBroken, ChapelWax, IcePillar, IceRubble, WaterPipe, SteamDrive, TallTree, FallenLog, LogBridge, LightTower, TowerWreck, Pallet, PalletStack, BoundRocks, ContainerSide, BearBed, BonePile, IceSpikes, SnowPile, IceArchFoot, ForestWeb, Count };

constexpr bool bridge_prop(PropKind kind) {return kind==PropKind::BridgePlank || kind==PropKind::LogBridge;}

// STORAGE: One compact prop per tile, independent of actor slots and inventories.
struct Prop {
    PropKind kind = PropKind::None;
    std::uint8_t hp = 0;
    std::uint8_t variant = 0;
    bool broken = false;
    std::uint16_t growth_ticks = 0;
    bool covered = false;
};
static_assert(sizeof(Prop) == 8);

struct PropSpec {
    Sprite sprite;
    SoundId sound;
    int health;
    bool blocking;
    bool breaks_on_step;
    LightEmitter light{};
};

PropSpec prop_spec(PropKind kind);
int prop_max_health(const Prop& prop);
bool prop_blocks(const Prop& prop);
bool prop_low_cover(const Prop& prop);
bool prop_shoot_through(const Prop& prop);
bool prop_cuttable_metal(const Prop& prop);
