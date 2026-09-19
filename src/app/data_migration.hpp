#pragma once
#include <filesystem>
#include <string>

bool migrate_user_data(const std::filesystem::path& previous,
                       const std::filesystem::path& current, std::string& error);
