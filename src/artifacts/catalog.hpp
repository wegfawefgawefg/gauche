#pragma once
#include "../game.hpp"
#include <array>

inline constexpr std::array artifact_kinds{ArtifactKind::AllPiercing,
    ArtifactKind::Reflector, ArtifactKind::Hearth, ArtifactKind::FleetFeet};
const char* artifact_name(ArtifactKind kind);
const char* artifact_description(ArtifactKind kind);
const char* artifact_summary(ArtifactKind kind);
Sprite artifact_icon(ArtifactKind kind);
