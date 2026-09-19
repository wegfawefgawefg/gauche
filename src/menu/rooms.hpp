#pragma once

struct FrontPage;
namespace teeming_menu { class ViewBuilder; }

void rooms_page(teeming_menu::ViewBuilder& ui, const FrontPage& page);
void party_page(teeming_menu::ViewBuilder& ui, const FrontPage& page);

void online_host_page(teeming_menu::ViewBuilder& ui, const FrontPage& page);
void network_options_page(teeming_menu::ViewBuilder& ui, const FrontPage& page);
