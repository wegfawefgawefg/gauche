#include "system.hpp"
#include "../world/water.hpp"

#include <algorithm>
#include <cmath>

namespace {

float random_unit(AmbientAudio& audio) {
    audio.random ^= audio.random >> 12;
    audio.random ^= audio.random << 25;
    audio.random ^= audio.random >> 27;
    const auto value = static_cast<std::uint32_t>((audio.random * 2685821657736338717ULL) >> 32);
    return static_cast<float>(value & 0xffffU) / 65536.0F;
}

float source_gain(const AmbientSource& source, const Game& game, Cell listener) {
    if (source.owner.slot >= 0) {
        const Entity* owner = get_entity(game, source.owner);
        if (owner == nullptr || owner->health <= 0) return 0;
    }
    if (source.prop != PropKind::None) {
        const Tile* tile = game.stage.at(source.cell);
        if (tile == nullptr || tile->prop.kind != source.prop || tile->prop.broken) return 0;
    }
    // TERRAIN: A demolished pool must not keep playing its old water loop.
    if (source.cue == AmbientCue::Stream || source.cue == AmbientCue::WallTrickle ||
        source.cue == AmbientCue::PoolDrips || source.cue == AmbientCue::ReedHiss ||
        source.cue == AmbientCue::Frogs) {
        const Tile* tile = game.stage.at(source.cell);
        if (tile == nullptr || (tile->kind != TileKind::Water && !shallow_water(tile->kind))) return 0;
    }
    const AmbientSpec& spec = ambient_specs[static_cast<std::size_t>(source.cue)];
    if (source.global) return spec.gain;
    const float dx = static_cast<float>(listener.x - source.cell.x);
    const float dy = static_cast<float>(listener.y - source.cell.y);
    const float range = std::sqrt(dx * dx + dy * dy);
    const float t = std::clamp((range - spec.near_radius) / (spec.far_radius - spec.near_radius), 0.0F, 1.0F);
    return spec.gain * (1 - t) * (1 - t);
}

void pan_voice(AmbientVoice& voice, const AmbientSource& source, Cell listener, float volume) {
    const float pan = source.global ? 0 : std::clamp(static_cast<float>(source.cell.x - listener.x) / 10, -1.0F, 1.0F);
    const float angle = (pan + 1) * .78539816F;
    const MIX_StereoGains stereo{std::cos(angle), std::sin(angle)};
    MIX_SetTrackStereo(voice.track, &stereo);
    MIX_SetTrackGain(voice.track, voice.gain * volume);
}

void start_voice(AmbientAudio& audio, AmbientVoice& voice, int source, bool loop) {
    voice.source = source;
    voice.gain = 0;
    MIX_SetTrackAudio(voice.track, audio.samples[static_cast<std::size_t>(audio.sources[static_cast<std::size_t>(source)].cue)]);
    MIX_SetTrackGain(voice.track, 0);
    const SDL_PropertiesID props = loop ? SDL_CreateProperties() : 0;
    if (props != 0) SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    MIX_PlayTrack(voice.track, props);
    if (props != 0) SDL_DestroyProperties(props);
}

void schedule_events(AmbientAudio& audio, const Game& game, Cell listener) {
    constexpr float beat = .1F;
    audio.quiet_time = std::max(0.0F, audio.quiet_time - beat);
    for (std::size_t i = 0; i < audio.sources.size(); ++i) {
        AmbientSource& source = audio.sources[i];
        const AmbientSpec& spec = ambient_specs[static_cast<std::size_t>(source.cue)];
        source.cooldown = std::max(0.0F, source.cooldown - beat);
        const bool inside = static_cast<float>(distance(source.cell, listener)) <= spec.trigger_radius;
        const bool entered = inside && !source.inside;
        source.inside = inside;
        if (entered) source.pending = true;
        if (!inside) source.pending = false;
        if (spec.mode == AmbientMode::Loop || source.cooldown > 0 ||
            (source.consumed && !spec.rearm) || source_gain(source, game, listener) < .005F) continue;
        const bool fire = spec.mode == AmbientMode::Enter ? source.pending :
            random_unit(audio) < 1.0F - std::exp(-spec.chance_per_second * beat);
        if (!fire || audio.quiet_time > 0) continue;
        auto voice = std::find_if(audio.events.begin(), audio.events.end(),
            [](const AmbientVoice& candidate) { return !MIX_TrackPlaying(candidate.track); });
        if (voice == audio.events.end()) continue;
        start_voice(audio, *voice, static_cast<int>(i), false);
        voice->gain = source_gain(source, game, listener);
        source.cooldown = spec.cooldown * (.8F + .4F * random_unit(audio));
        source.consumed = spec.mode == AmbientMode::Enter;
        source.pending = false;
        audio.quiet_time = 2.5F;
    }
}

} // namespace

bool init_ambience(AmbientAudio& audio, MIX_Mixer* mixer,
                   const std::filesystem::path& root, std::string& error) {
    audio.random = SDL_GetTicksNS() | 1;
    for (std::size_t i = 0; i < ambient_specs.size(); ++i) {
        const auto path = root / "ambience" / (std::string{ambient_specs[i].name} + ".ogg");
        audio.samples[i] = MIX_LoadAudio(mixer, path.string().c_str(), true);
        if (audio.samples[i] == nullptr) { error = "Cannot load " + path.string() + ": " + SDL_GetError(); return false; }
    }
    for (AmbientVoice& voice : audio.loops) {
        voice.track = MIX_CreateTrack(mixer);
        if (voice.track == nullptr) { error = SDL_GetError(); return false; }
    }
    for (AmbientVoice& voice : audio.events) {
        voice.track = MIX_CreateTrack(mixer);
        if (voice.track == nullptr) { error = SDL_GetError(); return false; }
    }
    return true;
}

void shutdown_ambience(AmbientAudio& audio) {
    for (AmbientVoice& voice : audio.loops) {
        if (voice.track != nullptr) { MIX_StopTrack(voice.track, 0); MIX_DestroyTrack(voice.track); }
        voice = {};
    }
    for (AmbientVoice& voice : audio.events) {
        if (voice.track != nullptr) { MIX_StopTrack(voice.track, 0); MIX_DestroyTrack(voice.track); }
        voice = {};
    }
    for (MIX_Audio*& sample : audio.samples) {
        if (sample != nullptr) MIX_DestroyAudio(sample);
        sample = nullptr;
    }
    audio.sources.clear();
    audio.world_key = 0;
}

void update_ambience(AmbientAudio& audio, const Game& game, Cell listener,
                     float seconds, float volume, bool enabled) {
    const std::uint64_t key = game.run.seed ^ (static_cast<std::uint64_t>(game.run.floor) << 48) ^
        (static_cast<std::uint64_t>(game.stage.width) << 32) ^ static_cast<std::uint64_t>(game.stage.height);
    if (key != audio.world_key) {
        for (AmbientVoice& voice : audio.loops) { MIX_StopTrack(voice.track, 0); voice.source = -1; voice.gain = 0; }
        for (AmbientVoice& voice : audio.events) { MIX_StopTrack(voice.track, 0); voice.source = -1; voice.gain = 0; }
        audio.world_key = key;
        audio.quiet_time = 3;
        audio.scheduler = 0;
        place_ambience(audio, game, listener);
    }
    const float delta = std::clamp(seconds, 0.0F, .1F);
    // VOICES: Six loops have their own budget; distant streams never steal gunshots.
    std::vector<int> choices;
    if (enabled) for (std::size_t i = 0; i < audio.sources.size(); ++i)
        if (ambient_specs[static_cast<std::size_t>(audio.sources[i].cue)].mode == AmbientMode::Loop &&
            source_gain(audio.sources[i], game, listener) > .003F) choices.push_back(static_cast<int>(i));
    std::stable_sort(choices.begin(), choices.end(), [&](int a, int b) {
        return source_gain(audio.sources[static_cast<std::size_t>(a)], game, listener) >
               source_gain(audio.sources[static_cast<std::size_t>(b)], game, listener);
    });
    if (choices.size() > audio.loops.size()) choices.resize(audio.loops.size());
    for (AmbientVoice& voice : audio.loops) {
        const bool wanted = std::find(choices.begin(), choices.end(), voice.source) != choices.end();
        const float target = wanted ? source_gain(audio.sources[static_cast<std::size_t>(voice.source)], game, listener) : 0;
        voice.gain += (target - voice.gain) * std::min(1.0F, delta * 4);
        if (voice.source >= 0) pan_voice(voice, audio.sources[static_cast<std::size_t>(voice.source)], listener, volume);
        if (!wanted && voice.gain < .001F) { MIX_StopTrack(voice.track, 0); voice.source = -1; }
    }
    for (int choice : choices) {
        if (std::any_of(audio.loops.begin(), audio.loops.end(), [choice](const AmbientVoice& v) { return v.source == choice; })) continue;
        auto voice = std::find_if(audio.loops.begin(), audio.loops.end(), [](const AmbientVoice& v) { return v.source < 0; });
        if (voice != audio.loops.end()) start_voice(audio, *voice, choice, true);
    }
    if (enabled) {
        audio.scheduler += delta;
        while (audio.scheduler >= .1F) { schedule_events(audio, game, listener); audio.scheduler -= .1F; }
    }
    for (AmbientVoice& voice : audio.events) {
        if (voice.source < 0) continue;
        const float target = enabled ? source_gain(audio.sources[static_cast<std::size_t>(voice.source)], game, listener) : 0;
        voice.gain += (target - voice.gain) * std::min(1.0F, delta * 5);
        pan_voice(voice, audio.sources[static_cast<std::size_t>(voice.source)], listener, volume);
    }
}
