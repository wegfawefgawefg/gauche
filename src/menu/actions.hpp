#pragma once

#include <string_view>

struct MenuShell;

void initialize_menu_settings(MenuShell& menu);
void apply_menu_action(MenuShell& menu, std::string_view action);
