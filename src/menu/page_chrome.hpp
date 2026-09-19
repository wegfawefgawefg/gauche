#pragma once
#include "view_builder.hpp"

namespace teeming_menu {
void frame(ViewBuilder& ui, std::string title, float width = 760, float height = 520);
void button(ViewBuilder& ui, std::string id, std::string label,
            std::string action, float height = 46);
void footer(ViewBuilder& ui, std::string first);
}
