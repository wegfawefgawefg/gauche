#include "http.hpp"
#include "../net/rooms/http.hpp"
#include <emscripten.h>
#include <cstdlib>
#include <map>

namespace {
EM_JS(int, start_fetch, (const char* address, const char* body), {
    const url = UTF8ToString(address), payload = body ? UTF8ToString(body) : null;
    const id = (Module.httpSequence = (Module.httpSequence || 0) + 1);
    Module.httpReplies ??= new Map();
    Module.httpReplies.set(id, null);
    const options = {signal: AbortSignal.timeout(5000), credentials: 'omit', cache: 'no-store'};
    if (payload !== null) Object.assign(options, {method: 'POST', headers: {'Content-Type': 'application/json'}, body: payload});
    fetch(url, options).then(async response => {
        const date = Date.parse(response.headers.get('Date'));
        const text = await response.text();
        if (text.length > 2097152) throw new Error('Room response too large');
        if (Module.httpReplies.has(id)) Module.httpReplies.set(id, JSON.stringify({
            status: response.status, body: text, date: Number.isFinite(date) ? date : 0
        }));
    }).catch(error => {
        if (Module.httpReplies.has(id)) Module.httpReplies.set(id, JSON.stringify({error: String(error)}));
    });
    return id;
});
EM_JS(char*, read_fetch, (int id), {
    const reply = Module.httpReplies.get(id);
    return reply ? stringToNewUTF8(reply) : 0;
});
EM_JS(void, clear_fetches, (), { Module.httpReplies?.clear(); });
EM_JS(char*, parse_host, (const char* address), {
    try { return stringToNewUTF8(new URL(UTF8ToString(address)).hostname); }
    catch (_) { return stringToNewUTF8(""); }
});
struct Reply { int id=0; std::optional<nlohmann::json> value; ServerClock clock; };
std::map<std::string, Reply> replies;
std::optional<nlohmann::json> request(const std::string& server,const std::string& path,
    const nlohmann::json* body,std::string& error,ServerClock* clock) {
    if (!server.starts_with("https://") && !server.starts_with("http://localhost") &&
        !server.starts_with("http://127.0.0.1")) {
        error="Room service needs HTTPS"; return {};
    }
    std::string url=server;
    while(url.ends_with('/'))url.pop_back();
    url+=path;
    const std::string payload=body ? body->dump() : "";
    auto& reply=replies[url+"\n"+payload];
    if(reply.id==0)reply.id=start_fetch(url.c_str(),body ? payload.c_str() : nullptr);
    if(!reply.value) {
        char* raw=read_fetch(reply.id);
        if(!raw)throw BrowserHttpPending{};
        reply.value=nlohmann::json::parse(raw,nullptr,false);
        std::free(raw);
        if(reply.value->is_object())reply.clock={reply.value->value("date",std::uint64_t{0}),steady_milliseconds()};
    }
    const auto& result=*reply.value;
    if(!result.is_object()){error="Invalid HTTP result";return {};}
    if(result.contains("error")){error=result.value("error","");return {};}
    const int status=result.value("status",0);
    if(status<200||status>=300){error="Room service HTTP "+std::to_string(status);return {};}
    auto parsed=nlohmann::json::parse(result.value("body",""),nullptr,false);
    if(!parsed.is_object()){error="Invalid room service response";return {};}
    if(clock)*clock=reply.clock;
    return parsed;
}
}
void clear_browser_http(){replies.clear();clear_fetches();}
std::optional<nlohmann::json> get_json(const std::string& server,const std::string& path,std::string& error,ServerClock* clock) {
    return request(server,path,nullptr,error,clock);
}
std::optional<nlohmann::json> post_json(const std::string& server,const std::string& path,const nlohmann::json& body,std::string& error) {
    return request(server,path,&body,error,nullptr);
}
std::string room_server_host(const std::string& url) {
    char* raw=parse_host(url.c_str());std::string host=raw;std::free(raw);return host;
}
