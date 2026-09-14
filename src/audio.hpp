#pragma once

#include "game.hpp"
#include "ambience/system.hpp"

#include <SDL3_mixer/SDL_mixer.h>

#include <array>
#include <filesystem>
#include <string>

struct GameAudio {
    AmbientAudio ambience;
    MIX_Mixer* mixer = nullptr;
    MIX_Track* music_track = nullptr;
    std::array<MIX_Track*, 12> tracks{};
    std::array<MIX_Audio*, static_cast<std::size_t>(SoundId::Count)> sounds{};
    std::array<MIX_Audio*, 2> songs{};
    std::array<std::uint64_t, 256> played_events{};
    std::size_t next_track = 0;
    std::size_t next_event = 0;
    int current_song = -1;
    float master_level = 1.0F;
    float music_level = 1.0F;
    float sound_level = 1.0F;
    bool initialized = false;
    GameAudio() = default;
    GameAudio(const GameAudio&) = delete;
    GameAudio& operator=(const GameAudio&) = delete;
    ~GameAudio();
};

bool init_audio(GameAudio& audio, const std::filesystem::path& root, std::string& error);
void shutdown_audio(GameAudio& audio);
void play_song(GameAudio& audio, int song);
void play_game_sounds(GameAudio& audio, const Game& game, Cell listener);
void sync_audio_settings(GameAudio& audio, const std::filesystem::path& path);
