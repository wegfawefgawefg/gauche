#pragma once
#include <gview/types.hpp>

struct FrontPage;
struct GameAudio;
enum class MenuSound { Move, Activate, Back, Change };
void play_menu_sound(GameAudio& audio, MenuSound sound, float gain);
void menu_feedback(FrontPage& page, gview::FeedbackEvent event);
void menu_back_sound(FrontPage& page);
