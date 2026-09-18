#pragma once
#include "../game.hpp"
#include <array>
inline constexpr std::array artifact_kinds{ArtifactKind::AllPiercing, ArtifactKind::Reflector, ArtifactKind::Hearth, ArtifactKind::FleetFeet, ArtifactKind::StrongArms, ArtifactKind::QuickHands, ArtifactKind::Vitality, ArtifactKind::Dodge, ArtifactKind::Regeneration, ArtifactKind::CriticalChance, ArtifactKind::CriticalPower, ArtifactKind::Armor, ArtifactKind::Technical, ArtifactKind::Medic, ArtifactKind::Reusable, ArtifactKind::Golddigger, ArtifactKind::Oversized, ArtifactKind::Sweeping, ArtifactKind::Iron, ArtifactKind::Chef, ArtifactKind::GodHand};
const char* artifact_name(ArtifactKind kind);
const char* artifact_description(ArtifactKind kind);
const char* artifact_summary(ArtifactKind kind);
Sprite artifact_icon(ArtifactKind kind);
