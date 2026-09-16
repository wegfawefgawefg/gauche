#pragma once
#include "../game.hpp"
#include <SDL3_mixer/SDL_mixer.h>
#include <array>

enum class ConsumptionCue { None, Food, Drink };
struct BodyVoice { MIX_Track* track=nullptr; Handle owner{}; };
struct SnoreBeat { std::uint32_t generation=0; std::uint64_t next=0; };
struct PendingBurp { Handle owner{}; std::uint64_t due=0; };
struct BodyAudio {
    std::array<BodyVoice,2> snores{};
    std::array<BodyVoice,2> burps{};
    std::array<SnoreBeat,max_entities> sleepers{};
    std::array<PendingBurp,8> pending{};
    std::uint64_t last_tick=0,seed=0,next_snore=0;
    int floor=-1;
};
struct GameAudio;
ConsumptionCue consumption_cue(SoundId sound);
bool meal_burp(std::uint64_t key,std::uint64_t seed);
bool init_body_audio(GameAudio& audio,std::string& error);
void stop_body_audio(GameAudio& audio);
void queue_meal_burp(GameAudio& audio,const Game& game,const SoundEvent& event,std::uint64_t key);
void update_body_audio(GameAudio& audio,const Game& game,Cell listener,bool enabled);
