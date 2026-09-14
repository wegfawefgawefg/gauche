#pragma once
#include "front_page.hpp"
#include "view_builder.hpp"

namespace gauche_menu {
void controls_page(ViewBuilder&, const FrontPage&);
void profile_editor_page(ViewBuilder&, const FrontPage&);
void bindings_page(ViewBuilder&, const FrontPage&);
void input_options_page(ViewBuilder&, const FrontPage&);
void binding_detail_page(ViewBuilder&, const FrontPage&);
void binding_choices_page(ViewBuilder&, const FrontPage&);
}
