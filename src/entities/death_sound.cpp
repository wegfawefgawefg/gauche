#include "death_sound.hpp"

// MATERIAL AND VOICE: Keep species cues out of the health-damage pipeline.
SoundId entity_death_sound(EntityKind kind) {
    switch (kind) {
    case EntityKind::FrostGoblin: return SoundId::FrostGoblinDeath;
    case EntityKind::PipeGuard: return SoundId::PipeGuardDeath;
    case EntityKind::RivetGunner: return SoundId::GunnerDeath;
    case EntityKind::Strikebreaker: return SoundId::BreakerDeath;
    case EntityKind::PowderMonkey: return SoundId::PowderDeath;
    case EntityKind::Ember: return SoundId::StokerDeath;
    case EntityKind::Pickhand: case EntityKind::ShiftForeman: return SoundId::CrewDeath;
    case EntityKind::Sled: return SoundId::SledBreak;
    case EntityKind::IceAnchor: return SoundId::AnchorBreak;
    case EntityKind::BoilerPorter: return SoundId::PorterDeath;
    case EntityKind::BoilerTank: return SoundId::BoilerBreak;
    case EntityKind::IcicleSpider: return SoundId::SpiderDeath;
    case EntityKind::ShardColony: return SoundId::ShardBreak;
    case EntityKind::CandleKeeper: return SoundId::KeeperDeath;
    case EntityKind::SnowEffigy: return SoundId::EffigyDeath;
    case EntityKind::RailCart: return SoundId::CartBreak;
    case EntityKind::RailShunter: return SoundId::ShunterDeath;
    case EntityKind::TarChoir: return SoundId::ChoirDeath;
    case EntityKind::MoldThief: return SoundId::MoldThiefDeath;
    case EntityKind::CoalCutter: return SoundId::BeltBreak;
    case EntityKind::CastingMold: return SoundId::MoldBreak;
    case EntityKind::EmergencyPump: return SoundId::PumpBreak;
    case EntityKind::Counterweight: return SoundId::WeightBreak;
    case EntityKind::AshSleeper: return SoundId::AshDeath;
    case EntityKind::SlagSnail: return SoundId::SlagDeath;
    case EntityKind::FurnaceMoth: return SoundId::FurnaceMothDeath;
    case EntityKind::AuditClerk: return SoundId::ClerkDeath;
    case EntityKind::WalkingKiln: return SoundId::KilnDeath;
    case EntityKind::CableCrawler: return SoundId::CrawlerDeath;
    case EntityKind::PressureRat: return SoundId::RatDeath;
    case EntityKind::MagnetCrane: return SoundId::CraneDeath;
    case EntityKind::ArcWelder: return SoundId::WelderDeath;
    case EntityKind::Yeti: return SoundId::YetiDeath;
    case EntityKind::AvalancheRam: return SoundId::RamDeath;
    case EntityKind::WhiteoutDrummer: return SoundId::DrummerDeath;
    case EntityKind::SealThief: return SoundId::SealDeath;
    case EntityKind::FishingWidow: return SoundId::WidowDeath;
    case EntityKind::FrozenPilgrim: return SoundId::PilgrimDeath;
    case EntityKind::EchoHound: return SoundId::EchoDeath;
    case EntityKind::LensWarden: return SoundId::WardenDeath;
    case EntityKind::MirrorKnight: return SoundId::KnightDeath;
    case EntityKind::SnowBurrower: return SoundId::SnowDeath;
    case EntityKind::GlassEel: return SoundId::EelDeath;
    case EntityKind::IceMason: return SoundId::MasonDeath;
    case EntityKind::SteamLeech: return SoundId::LeechDeath;
    case EntityKind::BellDiver: return SoundId::DiverDeath;
    case EntityKind::FrostBat: return SoundId::FrostDeath;
    case EntityKind::RimeSkater: return SoundId::SkaterBreak;
    case EntityKind::WaspNest: return SoundId::NestBreak;
    case EntityKind::CrateMimic: case EntityKind::RootTurret:
    case EntityKind::BrambleGuard: return SoundId::WoodCrack;
    case EntityKind::ThornSnail: return SoundId::ShellKnock;
    default: return SoundId::AnimalCrush1;
    }
}
