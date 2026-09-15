#include "audio.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>

namespace {

// MIX: Leave room for combat tells and environmental sources at default sliders.
constexpr float music_mix_gain = 0.12F;

bool load_audio_asset(GameAudio& audio, MIX_Audio*& destination,
                      const std::filesystem::path& path, bool preload, std::string& error) {
    destination = MIX_LoadAudio(audio.mixer, path.string().c_str(), preload);
    if (destination != nullptr) return true;
    error = "Unable to load " + path.string() + ": " + SDL_GetError();
    return false;
}

float audio_level(std::string_view settings, std::string_view name) {
    const std::string key = "(" + std::string{name} + " ";
    const std::size_t at = settings.find(key);
    if (at == std::string_view::npos) return 1.0F;
    const char* begin = settings.data() + at + key.size();
    float value = 1.0F;
    const auto [end, error] = std::from_chars(begin, settings.data() + settings.size(), value);
    if (error != std::errc{} || end == begin || !std::isfinite(value)) return 1.0F;
    return std::clamp(value, 0.0F, 1.0F);
}

} // namespace

GameAudio::~GameAudio() { shutdown_audio(*this); }

bool init_audio(GameAudio& audio, const std::filesystem::path& root, std::string& error) {
    if (!MIX_Init()) {
        error = std::string{"MIX_Init failed: "} + SDL_GetError();
        return false;
    }
    audio.initialized = true;
    audio.mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (audio.mixer == nullptr) {
        error = std::string{"Unable to open audio device: "} + SDL_GetError();
        shutdown_audio(audio);
        return false;
    }
    if (!init_ambience(audio.ambience, audio.mixer, root, error)) {
        shutdown_audio(audio);
        return false;
    }
    audio.music_track = MIX_CreateTrack(audio.mixer);
    audio.menu_track = MIX_CreateTrack(audio.mixer);
    if (audio.music_track == nullptr || audio.menu_track == nullptr) {
        error = std::string{"Unable to create music track: "} + SDL_GetError();
        shutdown_audio(audio);
        return false;
    }
    for (MIX_Track*& track : audio.tracks) {
        track = MIX_CreateTrack(audio.mixer);
        if (track == nullptr) {
            error = std::string{"Unable to create sound track: "} + SDL_GetError();
            shutdown_audio(audio);
            return false;
        }
    }
    for (std::size_t index = 0; index < sound_names.size(); ++index) {
        const auto path = root / "sounds" / (std::string{sound_names[index]} + ".ogg");
        if (!load_audio_asset(audio, audio.sounds[index], path, true, error)) {
            shutdown_audio(audio);
            return false;
        }
    }
    constexpr const char* menu_names[]{"menu_move", "menu_activate", "menu_back", "menu_change"};
    for (std::size_t index = 0; index < audio.menu_sounds.size(); ++index)
        if (!load_audio_asset(audio, audio.menu_sounds[index], root / "sounds" /
                (std::string{menu_names[index]} + ".ogg"), true, error)) {
            shutdown_audio(audio); return false;
        }
    for (int index = 0; index < 2; ++index) {
        const auto path = root / "music" / (index == 0 ? "title.ogg" : "playing.ogg");
        if (!load_audio_asset(audio, audio.songs[static_cast<std::size_t>(index)],
                              path, false, error)) {
            shutdown_audio(audio);
            return false;
        }
    }
    return true;
}

void shutdown_audio(GameAudio& audio) {
    if (!audio.initialized) return;
    shutdown_ambience(audio.ambience);
    if (audio.music_track != nullptr) MIX_StopTrack(audio.music_track, 0);
    for (MIX_Track* track : audio.tracks)
        if (track != nullptr) MIX_StopTrack(track, 0);
    for (MIX_Audio*& sound : audio.sounds) {
        if (sound != nullptr) MIX_DestroyAudio(sound);
        sound = nullptr;
    }
    if (audio.menu_track != nullptr) MIX_StopTrack(audio.menu_track, 0);
    for (MIX_Audio*& sound : audio.menu_sounds) {
        if (sound != nullptr) MIX_DestroyAudio(sound);
        sound = nullptr;
    }
    for (MIX_Audio*& song : audio.songs) {
        if (song != nullptr) MIX_DestroyAudio(song);
        song = nullptr;
    }
    if (audio.mixer != nullptr) MIX_DestroyMixer(audio.mixer);
    audio.mixer = nullptr;
    audio.music_track = nullptr;
    audio.menu_track = nullptr;
    audio.tracks.fill(nullptr);
    audio.current_song = -1;
    audio.initialized = false;
    MIX_Quit();
}

void play_song(GameAudio& audio, int song) {
    if (!audio.initialized || song < 0 || song >= 2 || song == audio.current_song) return;
    MIX_StopTrack(audio.music_track, 0);
    MIX_SetTrackAudio(audio.music_track, audio.songs[static_cast<std::size_t>(song)]);
    MIX_SetTrackGain(audio.music_track,
                     music_mix_gain * audio.master_level * audio.music_level);
    const SDL_PropertiesID properties = SDL_CreateProperties();
    if (properties != 0) SDL_SetNumberProperty(properties, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    MIX_PlayTrack(audio.music_track, properties);
    if (properties != 0) SDL_DestroyProperties(properties);
    audio.current_song = song;
}

void play_game_sounds(GameAudio& audio, const Game& game, Cell listener) {
    if (!audio.initialized) return;
    constexpr float half_pi = 1.57079632679F;
    for (int index = 0; index < game.sound_count; ++index) {
        const SoundEvent& event = game.sounds[static_cast<std::size_t>(index)];
        const std::uint64_t key = (event.tick << 8) |
            (static_cast<std::uint64_t>(event.sequence) + 1);
        if (std::find(audio.played_events.begin(), audio.played_events.end(), key) !=
            audio.played_events.end()) continue;
        audio.played_events[audio.next_event++ % audio.played_events.size()] = key;

        float volume = 0.9F * audio.master_level * audio.sound_level * (event.muffled ? .25F : 1.0F);
        MIX_StereoGains stereo{1.0F, 1.0F};
        if (event.positional) {
            const float dx = static_cast<float>(event.cell.x - listener.x);
            const float dy = static_cast<float>(event.cell.y - listener.y);
            const float distance_to_sound = std::sqrt(dx * dx + dy * dy);
            if (distance_to_sound >= 16.0F) continue;
            volume *= 1.0F - distance_to_sound / 16.0F;
            const float pan = std::clamp(dx / 8.0F, -1.0F, 1.0F);
            const float angle = (pan + 1.0F) * half_pi * 0.5F;
            stereo = {std::cos(angle), std::sin(angle)};
        }
        MIX_Track* track = nullptr;
        for (std::size_t offset = 0; offset < audio.tracks.size(); ++offset) {
            const std::size_t slot = (audio.next_track + offset) % audio.tracks.size();
            if (!MIX_TrackPlaying(audio.tracks[slot])) {
                track = audio.tracks[slot];
                audio.next_track = (slot + 1) % audio.tracks.size();
                break;
            }
        }
        if (track == nullptr) {
            track = audio.tracks[audio.next_track];
            audio.next_track = (audio.next_track + 1) % audio.tracks.size();
        }
        MIX_SetTrackAudio(track, audio.sounds[static_cast<std::size_t>(event.sound)]);
        MIX_SetTrackGain(track, volume);
        MIX_SetTrackStereo(track, &stereo);
        MIX_PlayTrack(track, 0);
    }
}

void sync_audio_settings(GameAudio& audio, const std::filesystem::path& path) {
    std::ifstream file(path);
    const std::string contents = file ?
        std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}} : "";
    const float master = audio_level(contents, "master");
    const float music = audio_level(contents, "music");
    const float sound = audio_level(contents, "sfx");
    if (master == audio.master_level && music == audio.music_level &&
        sound == audio.sound_level) return;
    audio.master_level = master;
    audio.music_level = music;
    audio.sound_level = sound;
    if (audio.music_track != nullptr)
        MIX_SetTrackGain(audio.music_track, music_mix_gain * master * music);
}
