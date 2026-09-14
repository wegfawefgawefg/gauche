#pragma once

#include <string_view>

struct FrontPage;
struct EngineState;
#include <gubsy/input/binds_profile.hpp>

void ensure_input_pair(EngineState& engine, const BindsProfile& binds);
bool default_profile(const BindsProfile& profile);
bool profile_read_only(const FrontPage& page);
bool save_profile_name(FrontPage& page);
bool profile_action(FrontPage& page, std::string_view action);
bool binding_action(FrontPage& page, std::string_view action);
int active_profile_id(const FrontPage& page);
