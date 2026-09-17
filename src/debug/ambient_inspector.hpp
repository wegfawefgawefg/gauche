#pragma once
#include "../ambience/system.hpp"
#include "../render.hpp"

// Local presentation only. Preview sources have no mixer tracks or scheduler.
struct AmbientInspector {
    bool visible=false,overlay=false,labels=true,ranges=true,triggers=true,mute=false;
    bool selected_only=false;
    int mode=0,selected=-1;
    bool preview=false;
    Cell listener{};
    const Game* game=nullptr;
    const AmbientAudio* audio=nullptr;
    AmbientAudio preview_audio;
    const Game* preview_game=nullptr;
    std::weak_ptr<const GenerationReport> report;
    std::uint64_t world_key=0;
};
AmbientInspector& ambient_inspector();
void prepare_ambient_inspector(const Game& live,const AmbientAudio& audio,Cell listener);
void draw_ambient_inspector();
void draw_ambient_annotations(SDL_Renderer* renderer,const Game& game,ViewCamera camera,float zoom);
