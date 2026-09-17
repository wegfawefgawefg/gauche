#include "http.hpp"

#include <curl/curl.h>
#include <limits>
#include <string_view>

namespace {

bool initialize_curl() {
    static const bool ready = curl_global_init(CURL_GLOBAL_DEFAULT) == CURLE_OK;
    return ready;
}

std::size_t receive_body(char* bytes, std::size_t size, std::size_t count, void* data) {
    auto& body = *static_cast<std::string*>(data);
    if (size != 0 && count > std::numeric_limits<std::size_t>::max() / size) return 0;
    const auto amount = size * count;
    if (amount > 2 * 1024 * 1024 || body.size() + amount > 2 * 1024 * 1024) return 0;
    body.append(bytes, amount);
    return amount;
}

std::size_t receive_header(char* bytes, std::size_t size, std::size_t count, void* data) {
    if (size != 0 && count > std::numeric_limits<std::size_t>::max() / size) return 0;
    const auto amount = size * count;
    auto& clock = *static_cast<ServerClock*>(data);
    const std::string_view header(bytes, amount);
    // An interim response must not supply the final response's clock.
    if (header.starts_with("HTTP/")) clock = {};
    if (amount >= 5 && curl_strnequal(bytes, "Date:", 5)) {
        const std::string date(header.substr(5));
        const auto stamp = curl_getdate(date.c_str(), nullptr);
        if (stamp > 0) clock = {static_cast<std::uint64_t>(stamp) * 1000, steady_milliseconds()};
    }
    return amount;
}

std::optional<nlohmann::json> request(const std::string& server, const std::string& path,
                                    const nlohmann::json* json, std::string& error, ServerClock* clock = nullptr) {
    if (clock) *clock = {};
    if (!initialize_curl()) { error = "HTTP initialization failed"; return {}; }
    if (!server.starts_with("https://") && !server.starts_with("http://")) {
        error = "Room service needs an http:// or https:// address"; return {};
    }
    CURL* curl = curl_easy_init();
    if (curl == nullptr) { error = "HTTP request allocation failed"; return {}; }
    std::string url = server;
    while (url.ends_with('/')) url.pop_back();
    url += path;
    std::string response;
    ServerClock response_clock;
    const std::string body = json ? json->dump() : "";
    curl_slist* headers = curl_slist_append(nullptr, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_PROTOCOLS_STR, "http,https");
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 2000L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 4000L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, receive_body);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, receive_header);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_clock);
    if (json) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
    }
    // TLS: Keep libcurl's certificate and hostname verification enabled.
    const auto result = curl_easy_perform(curl);
    long status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (result != CURLE_OK) { error = std::string("Room service: ") + curl_easy_strerror(result); return {}; }
    if (status < 200 || status >= 300) {
        error = "Room service HTTP " + std::to_string(status); return {};
    }
    auto parsed = nlohmann::json::parse(response, nullptr, false);
    if (!parsed.is_object()) { error = "Invalid room service response"; return {}; }
    if (clock) *clock = response_clock;
    return parsed;
}

} // namespace

std::optional<nlohmann::json> get_json(const std::string& url, const std::string& path, std::string& error, ServerClock* clock) {
    return request(url, path, nullptr, error, clock);
}

std::optional<nlohmann::json> post_json(const std::string& url, const std::string& path,
                                      const nlohmann::json& body, std::string& error) {
    return request(url, path, &body, error);
}

std::string room_server_host(const std::string& url) {
    if (!initialize_curl()) return {};
    CURLU* parsed = curl_url();
    if (parsed == nullptr) return {};
    char* host = nullptr;
    std::string result;
    if (curl_url_set(parsed, CURLUPART_URL, url.c_str(), 0) == CURLUE_OK &&
        curl_url_get(parsed, CURLUPART_HOST, &host, 0) == CURLUE_OK) result = host;
    curl_free(host);
    curl_url_cleanup(parsed);
    return result;
}
