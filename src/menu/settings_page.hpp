#pragma once
#include "view_builder.hpp"
struct FrontPage;
namespace gauche_menu {
void settings_page(ViewBuilder& ui, const FrontPage& page);
void display_page(ViewBuilder& ui, const FrontPage& page);
void audio_page(ViewBuilder& ui, const FrontPage& page);
}
