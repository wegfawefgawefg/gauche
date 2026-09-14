#pragma once

#include "catalog.hpp"
#include "../game.hpp"

#include <SDL3_mixer/SDL_mixer.h>
#include <filesystem>
#include <vector>

struct AmbientSource {
    AmbientCue cue{};
    Cell cell{};
    Handle owner{};
    PropKind prop = PropKind::None;
    float cooldown = 0;
    bool global = false;
    bool inside = false;
    bool consumed = false;
    bool pending = false;
};
struct AmbientVoice {
    MIX_Track* track = nullptr;
    int source = -1;
    float gain = 0;
};
struct AmbientAudio {
    std::array<MIX_Audio*, static_cast<std::size_t>(AmbientCue::Count)> samples{};
    std::array<AmbientVoice, 6> loops{};
    std::array<AmbientVoice, 3> events{};
    std::vector<AmbientSource> sources;
    std::uint64_t world_key = 0;
    std::uint64_t random = 1;
    float scheduler = 0;
    float quiet_time = 3;
};

bool init_ambience(AmbientAudio& audio, MIX_Mixer* mixer,
                   const std::filesystem::path& root, std::string& error);
void shutdown_ambience(AmbientAudio& audio);
void place_ambience(AmbientAudio& audio, const Game& game, Cell listener);
void update_ambience(AmbientAudio& audio, const Game& game, Cell listener,
                     float seconds, float volume, bool enabled);
