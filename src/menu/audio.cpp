#include "audio.hpp"
#include "front_page.hpp"
#include "../audio.hpp"
#include "src/engine_state.hpp"

// LOCAL FEEDBACK: One quiet menu voice; hovering the same row never retriggers it.
void play_menu_sound(GameAudio& audio, MenuSound sound, float gain) {
    if (!audio.initialized || audio.menu_track == nullptr) return;
    const auto now = SDL_GetTicks();
    if ((sound == MenuSound::Move || sound == MenuSound::Change) &&
        now - audio.last_menu_sound < 65) return;
    audio.last_menu_sound = now;
    MIX_SetTrackAudio(audio.menu_track, audio.menu_sounds[static_cast<std::size_t>(sound)]);
    MIX_SetTrackGain(audio.menu_track, gain * .48F);
    MIX_PlayTrack(audio.menu_track, 0);
}

void menu_feedback(FrontPage& page, gview::FeedbackEvent event) {
    if (page.audio == nullptr || page.backend == nullptr) return;
    MenuSound sound = MenuSound::Move;
    switch (event) {
    case gview::FeedbackEvent::Move: break;
    case gview::FeedbackEvent::Activate: case gview::FeedbackEvent::Open:
        sound = MenuSound::Activate; break;
    case gview::FeedbackEvent::Close: case gview::FeedbackEvent::Reject:
        sound = MenuSound::Back; break;
    case gview::FeedbackEvent::Toggle: sound = MenuSound::Change; break;
    }
    const auto& levels = gubsy_runtime_engine(*page.backend).audio_settings;
    play_menu_sound(*page.audio, sound, levels.vol_master * levels.vol_sfx);
}

void menu_back_sound(FrontPage& page) { menu_feedback(page, gview::FeedbackEvent::Close); }
