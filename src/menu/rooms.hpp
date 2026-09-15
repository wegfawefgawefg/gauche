#pragma once

struct FrontPage;
namespace gauche_menu { class ViewBuilder; }

void rooms_page(gauche_menu::ViewBuilder& ui, const FrontPage& page);
void party_page(gauche_menu::ViewBuilder& ui, const FrontPage& page);
