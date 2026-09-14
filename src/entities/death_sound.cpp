#include "death_sound.hpp"

// MATERIAL AND VOICE: Keep species cues out of the health-damage pipeline.
SoundId entity_death_sound(EntityKind kind) {
    switch (kind) {
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
