#include "catalog.hpp"

// PASSIVES: One vocabulary for the reward offer and the player's owned effects.
const char* artifact_name(ArtifactKind kind) {
    switch (kind) {
    case ArtifactKind::AllPiercing: return "All Piercing";
    case ArtifactKind::Reflector: return "Reflector";
    case ArtifactKind::Hearth: return "Hearth";
    case ArtifactKind::FleetFeet: return "Fleet Feet";
    case ArtifactKind::None: return "None";
    }
    return "Unknown";
}

const char* artifact_description(ArtifactKind kind) {
    switch (kind) {
    case ArtifactKind::AllPiercing: return "Shots pass through actors until a wall. Ignore directional enemy armor.";
    case ArtifactKind::Reflector: return "25% chance to return half of a survived, blockable hit to its attacker.";
    case ArtifactKind::Hearth: return "Heal yourself and friends within four tiles by 1 HP each second.";
    case ArtifactKind::FleetFeet: return "Move one tile about 0.03 seconds sooner, down to 0.05 seconds per step.";
    case ArtifactKind::None: return "";
    }
    return "";
}

const char* artifact_summary(ArtifactKind kind) {
    switch (kind) {
    case ArtifactKind::AllPiercing: return "Pierce actors and armor";
    case ArtifactKind::Reflector: return "25%: return half a hit";
    case ArtifactKind::Hearth: return "+1 HP/s in 4-tile aura";
    case ArtifactKind::FleetFeet: return "Step -0.033s; min 0.05s";
    case ArtifactKind::None: return "";
    }
    return "";
}

Sprite artifact_icon(ArtifactKind kind) {
    switch (kind) {
    case ArtifactKind::AllPiercing: return Sprite::Musket;
    case ArtifactKind::Reflector: return Sprite::Buckler;
    case ArtifactKind::Hearth: return Sprite::Campfire;
    case ArtifactKind::FleetFeet: return Sprite::PlayerFootprint;
    case ArtifactKind::None: return Sprite::Fist;
    }
    return Sprite::Fist;
}
