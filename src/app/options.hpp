#pragma once

#include "../game.hpp"
#include <gubsy/runtime.hpp>
#include <filesystem>
#include <optional>
#include <string_view>

bool wants_smoke(int argc, char** argv);
bool has_arg(int argc, char** argv, std::string_view name);
const char* capture_arg(int argc, char** argv);
std::string_view value_arg(int argc, char** argv, std::string_view name);
std::optional<int> number_arg(std::string_view text);
std::optional<float> decimal_arg(std::string_view text);
DeathPolicy requested_death_policy(int argc, char** argv);
std::filesystem::path user_data_root();
GubsyAppConfig app_config(int argc = 0, char** argv = nullptr);
