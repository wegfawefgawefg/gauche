#include "../items/echo_pebble.hpp"
#include "echo_hound.hpp"
#include "hearing.hpp"

#include <algorithm>

void hear_echo_hounds(Game& game, Cell origin, int radius) {
    // CULL: Most footsteps have no listener; avoid a flood in that common case.
    const auto nearby = [origin, radius](const Entity& actor) {
        return actor.kind == EntityKind::EchoHound && actor.health > 0 &&
            actor.cell != origin && distance(actor.cell, origin) <= radius;
    };
    if (std::none_of(game.entities.begin(), game.entities.end(), nearby)) return;
    const auto heard = audible_cells(game, origin, radius);
    for (Entity& actor : game.entities) {
        if (!nearby(actor) || std::find(heard.begin(), heard.end(), actor.cell) == heard.end()) continue;
        actor.point_c = origin;
        actor.label_c = InvestigateNoise;
        actor.timer_c = 300;
        actor.sleep_ticks = 0;
    }
}

void hear_world_action(Game& game, SoundId sound, Cell origin) {
    // ACTIONS: Deterministic emitted events, before the cosmetic sound buffer cap.
    // Audio volume, local ambience and hounds' own calls cannot steer their hearing.
    int radius = 0;
    switch (sound) {
    case SoundId::BridgeBreak:
    case SoundId::ThawBurst:
    case SoundId::BoilerBreak:
    case SoundId::Explosion: case SoundId::Explosion1: case SoundId::Explosion2:
    case SoundId::Explosion3: case SoundId::ThunderCrack: case SoundId::PrismBurst:
        radius = 12; break;
    case SoundId::ForkRing: case SoundId::CrystalBreak:
    case SoundId::SpikeSink:
    case SoundId::FlareLaunch:
    case SoundId::BoilerWarn: case SoundId::BoilerVent:
    case SoundId::PistolShot: case SoundId::MusketShot: case SoundId::ShotgunShot:
    case SoundId::SmgShot: case SoundId::BlunderShot: case SoundId::RocketLaunch:
    case SoundId::LensFire: case SoundId::WardenFire: case SoundId::SmallLaser:
    case SoundId::WoodCrack: case SoundId::PotBreak: case SoundId::OpticBreak:
    case SoundId::ClockBreak: case SoundId::LampBreak: case SoundId::LensCaseBreak: case SoundId::IceBlockBreak:
        radius = 10; break;
    case SoundId::StillwaterRing:
    case SoundId::SiphonFire:
    case SoundId::PorterHeave: case SoundId::PorterWarn: case SoundId::PorterHit: case SoundId::PorterDeath:
    case SoundId::BoilerRoll: case SoundId::LockerOpen: case SoundId::KettleReady:
    case SoundId::SpiderCatch: case SoundId::SpiderDeath:
    case SoundId::DrumBeat1: case SoundId::DrumBeat2: case SoundId::DrumBeat3:
    case SoundId::VaneBreak: case SoundId::GlobeBreak: case SoundId::SealBark: case SoundId::CreelBreak:
    case SoundId::CoalLand: case SoundId::StoveBreak: case SoundId::StoveHit:
    case SoundId::HarpoonFire: case SoundId::HarpoonImpact:
    case SoundId::SluiceOpen: case SoundId::SluiceClose: case SoundId::SluiceWarning:
    case SoundId::WedgeFit: case SoundId::WedgeLift: case SoundId::WedgeHit: case SoundId::WedgeBreak:
    case SoundId::RockImpact: case SoundId::ArrowImpact: case SoundId::IceNeedleHit:
    case SoundId::ShardCharge: case SoundId::ShardPulse: case SoundId::ShardBreak:
    case SoundId::KeeperScold: case SoundId::KeeperWarn: case SoundId::KeeperCast:
    case SoundId::KeeperDeath: case SoundId::CabinetOpen:
    case SoundId::EffigyCreak: case SoundId::EffigyWarn: case SoundId::EffigyHit: case SoundId::EffigyDeath:
    case SoundId::RamPaw: case SoundId::RamBonk: case SoundId::RamHit:
    case SoundId::TinOpen: case SoundId::BrineSplash:
    case SoundId::BottleBreak: case SoundId::BombLand: case SoundId::IceBrickThrow:
    case SoundId::SnowSplat: case SoundId::CrossbowShot: case SoundId::BowRelease:
    case SoundId::BoomerangHit: case SoundId::BoomerangLand: case SoundId::PrismLand:
        radius = 7; break;
    case SoundId::SledDeploy: case SoundId::SledBreak:
    case SoundId::ShelterPack: case SoundId::ShelterBreak:
    case SoundId::AnchorSet: case SoundId::AnchorBreak:
    case SoundId::MaskRaise:
    case SoundId::ForkWindup:
    case SoundId::BridgeUnfold: case SoundId::BridgeSplash:
    case SoundId::ThawPlace: case SoundId::ThawFuse:
    case SoundId::SiphonDraw: case SoundId::SummerOpen:
    case SoundId::WireLay: case SoundId::WireCut: case SoundId::SpikePlant: case SoundId::SpikeBreak:
    case SoundId::FlareLand:
    case SoundId::SkateSlash: case SoundId::SkateCut: case SoundId::CramponsFit:
    case SoundId::FistWindup: case SoundId::StickWindup: case SoundId::PickaxeWindup:
    case SoundId::AxeSwing: case SoundId::SpearThrust: case SoundId::MaulSwing:
    case SoundId::RakeSweep: case SoundId::KnifeStab: case SoundId::TorchSwing:
    case SoundId::ValveFit: case SoundId::ValveRemove: case SoundId::SealantPatch:
    case SoundId::KettleFill: case SoundId::SpiderWeave: case SoundId::SpiderCut:
    case SoundId::ClawScratch: case SoundId::Punch1: case SoundId::ChiselJab: case SoundId::BlockLand:
    case SoundId::CreelHit: case SoundId::BumpWood: case SoundId::BumpStone:
        radius = 4; break;
    default: return;
    }
    record_echo_pebbles(game,sound,origin);
    hear_echo_hounds(game, origin, radius);
}
