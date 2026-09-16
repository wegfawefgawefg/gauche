#include "body.hpp"
#include "../audio.hpp"
#include <algorithm>
#include <cmath>

ConsumptionCue consumption_cue(SoundId sound) {
    switch (sound) {
    case SoundId::BrothSip:return ConsumptionCue::Drink;
    case SoundId::MeatMunch:case SoundId::HerbsChew:case SoundId::RootChew:
    case SoundId::ChiliBite:case SoundId::BreadMunch:case SoundId::FishEat:
    case SoundId::FishNibble:case SoundId::KelpChew:case SoundId::EggEat:
    case SoundId::FruitMunch:case SoundId::LunchEat:case SoundId::SealEat:
    case SoundId::SeedPeck:return ConsumptionCue::Food;
    default:return ConsumptionCue::None;
    }
}

bool meal_burp(std::uint64_t key,std::uint64_t seed) {
    // Audio-only variation: never advance the saved world random stream.
    std::uint64_t bits=key^(seed*0x9e3779b97f4a7c15ULL)^0x62757270ULL;
    bits^=bits>>30;bits*=0xbf58476d1ce4e5b9ULL;
    bits^=bits>>27;bits*=0x94d049bb133111ebULL;bits^=bits>>31;
    return bits%100<15;
}

bool init_body_audio(GameAudio& audio,std::string& error) {
    for (auto* group:{&audio.body.snores,&audio.body.burps}) for (auto& voice:*group) {
        voice.track=MIX_CreateTrack(audio.mixer);
        if (!voice.track) {error=std::string{"Unable to create body sound track: "}+SDL_GetError();return false;}
    }
    return true;
}

void stop_body_audio(GameAudio& audio) {
    for (auto* group:{&audio.body.snores,&audio.body.burps}) for (auto& voice:*group) {
        if (voice.track) MIX_StopTrack(voice.track,0);
        voice.owner={};
    }
    audio.body.pending={};
}

namespace {
void prepare(GameAudio& audio,const Game& game) {
    auto& body=audio.body;
    if (body.floor!=game.run.floor || body.seed!=game.run.seed || game.tick<body.last_tick) {
        stop_body_audio(audio);body.sleepers={};body.next_snore=game.tick;
    }
    body.floor=game.run.floor;body.seed=game.run.seed;body.last_tick=game.tick;
}
float nearby(Cell cell,Cell listener) {
    const float x=static_cast<float>(cell.x-listener.x),y=static_cast<float>(cell.y-listener.y);
    return std::max(0.0F,1-std::sqrt(x*x+y*y)/12);
}
void locate(GameAudio& audio,BodyVoice& voice,Cell cell,Cell listener,float gain) {
    MIX_SetTrackGain(voice.track,gain*nearby(cell,listener)*audio.master_level*audio.sound_level);
    const float angle=(std::clamp(static_cast<float>(cell.x-listener.x)/8,-1.0F,1.0F)+1)*.785398163F;
    const MIX_StereoGains stereo{std::cos(angle),std::sin(angle)};
    MIX_SetTrackStereo(voice.track,&stereo);
}
BodyVoice* free_voice(std::array<BodyVoice,2>& voices) {
    for (auto& voice:voices) if (voice.track && !MIX_TrackPlaying(voice.track)) return &voice;
    return nullptr;
}
void start(GameAudio& audio,BodyVoice& voice,const Game& game,Handle owner,Cell listener,SoundId sound,float gain) {
    const auto* actor=get_entity(game,owner);if (!actor) return;
    voice.owner=owner;MIX_SetTrackAudio(voice.track,audio.sounds[static_cast<std::size_t>(sound)]);
    locate(audio,voice,actor->cell,listener,gain);MIX_PlayTrack(voice.track,0);
}
}

void queue_meal_burp(GameAudio& audio,const Game& game,const SoundEvent& event,std::uint64_t key) {
    if (consumption_cue(event.sound)!=ConsumptionCue::Food || !meal_burp(key,game.run.seed)) return;
    prepare(audio,game);
    // Successful eating sounds identify the eater's cell, including animal bait.
    // Never attach a delayed voice to a pickup, fixture or reused entity slot.
    Handle owner{};
    for (int i=0;i<max_entities;++i) {
        const auto& actor=game.entities[static_cast<std::size_t>(i)];
        if (actor.kind!=EntityKind::None && actor.cell==event.cell && actor.health>0 && actor.move_interval>0) {
            owner={i,actor.generation};break;
        }
    }
    if (owner.slot<0) return;
    for (auto& pending:audio.body.pending) if (pending.owner.slot<0) {
        pending={owner,game.tick+30+key%19};return;
    }
}

void update_body_audio(GameAudio& audio,const Game& game,Cell listener,bool enabled) {
    if (!audio.initialized) return;
    prepare(audio,game);
    if (!enabled) {stop_body_audio(audio);return;}
    auto& body=audio.body;
    for (auto* group:{&body.snores,&body.burps}) for (auto& voice:*group) {
        if (!voice.track || !MIX_TrackPlaying(voice.track)) {voice.owner={};continue;}
        const auto* actor=get_entity(game,voice.owner);
        const bool snore=group==&body.snores;
        if (!actor || actor->health<=0 || (snore && actor->sleep_ticks<=0) || nearby(actor->cell,listener)<=0) {
            MIX_StopTrack(voice.track,0);voice.owner={};continue;
        }
        locate(audio,voice,actor->cell,listener,snore ? .48F : .75F);
    }
    for (auto& pending:body.pending) {
        if (pending.owner.slot<0 || game.tick<pending.due) continue;
        const auto* actor=get_entity(game,pending.owner);
        if (actor && actor->health>0 && nearby(actor->cell,listener)>0)
            if (auto* voice=free_voice(body.burps)) start(audio,*voice,game,pending.owner,listener,SoundId::Burp,.75F);
        pending={};
    }
    if (game.tick<body.next_snore) return;
    auto* voice=free_voice(body.snores);if (!voice) return;
    int nearest=13;Handle chosen{};
    for (int slot=0;slot<max_entities;++slot) {
        const auto& actor=game.entities[static_cast<std::size_t>(slot)];
        if (actor.kind==EntityKind::None || actor.health<=0 || actor.sleep_ticks<=0 || actor.move_interval<=0) continue;
        auto& beat=body.sleepers[static_cast<std::size_t>(slot)];
        if (beat.generation!=actor.generation) beat={actor.generation,0};
        if (game.tick<beat.next) continue;
        const int range=distance(actor.cell,listener);
        if (range<nearest) {nearest=range;chosen={slot,actor.generation};}
    }
    if (chosen.slot<0) return;
    auto& beat=body.sleepers[static_cast<std::size_t>(chosen.slot)];
    beat.next=game.tick+240+static_cast<unsigned>(chosen.slot)%121;
    body.next_snore=game.tick+90;
    start(audio,*voice,game,chosen,listener,(chosen.slot&1) ? SoundId::SnoreHigh : SoundId::SnoreLow,.48F);
}
