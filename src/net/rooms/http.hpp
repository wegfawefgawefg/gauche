#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

std::optional<nlohmann::json> get_json(const std::string& url, const std::string& path, std::string& error);
std::optional<nlohmann::json> post_json(const std::string& url, const std::string& path,
                                      const nlohmann::json& body, std::string& error);
std::string room_server_host(const std::string& url);
