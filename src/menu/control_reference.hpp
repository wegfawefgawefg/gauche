#pragma once
#include "renderer/device.hpp"
#include "front_page.hpp"
#include "view_builder.hpp"

void control_reference(gauche_menu::ViewBuilder& ui, std::string_view parent,
                       const FrontPage& page, int profile_id);
void draw_control_diagram(tr::Renderer* renderer, const gview::PaintCommand& command,
                          const FrontPage& page);
void update_control_preview(FrontPage& page);
