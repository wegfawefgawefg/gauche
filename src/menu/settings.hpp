#pragma once
#include <gview/gview.hpp>
#include <optional>
struct FrontPage;
struct MenuShell;
std::optional<gview::Value> read_menu_setting(const FrontPage& page, std::string_view key);
void apply_menu_setting(MenuShell& menu, std::string_view key, const gview::Value& value);
void apply_pending_settings(MenuShell& menu);
