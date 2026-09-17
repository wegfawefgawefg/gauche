#include "runtime.hpp"
#include "http.hpp"
#include "../../net_protocol.hpp"

namespace {

bool service_endpoints(GaucheMatchmaking& api, const std::string& url, RoomResult& result) {
    RoomServerCapabilities capabilities;
    if (!api.fetch_capabilities(url, capabilities, result.error, &result.clock)) return false;
    const std::string server = room_server_host(url);
    const auto resolve = [&](const RoomServerUdpServiceCapabilities& service, NetEndpoint& endpoint) {
        if (!service.enabled || service.port < 1 || service.port > 65535) return false;
        const auto& host = service.host.empty() || service.host == "0.0.0.0" ||
            service.host == "::" ? server : service.host;
        return resolve_endpoint(host, static_cast<std::uint16_t>(service.port), endpoint, result.error);
    };
    const bool punch = resolve(capabilities.punch_udp, result.punch);
    const bool relay = resolve(capabilities.relay_udp, result.relay);
    if (!punch && !relay) result.error = "Room service has neither NAT punch nor relay enabled";
    return punch || relay;
}

bool compatible(const MatchmakingRoom& room) {
    return room.contract.game_version == std::to_string(gameplay_version) &&
        room.contract.net_protocol == "gauche-" + std::to_string(wire_version);
}

} // namespace

RoomResult perform_room_request(const RoomRequest& request) {
    RoomResult result;
    result.operation = request.operation;
    GaucheMatchmaking api;
    // WORKER: Only immutable request copies cross threads; no SDL or simulation state.
    try {
        switch (request.operation) {
        case RoomOperation::Browse:
            result.okay = api.list_rooms(request.url, result.rooms, result.error);
            std::erase_if(result.rooms, [](const auto& room) {
                return !room.contract.net_protocol.starts_with("gauche-");
            });
            break;
        case RoomOperation::Create:
            if (!service_endpoints(api, request.url, result)) break;
            result.okay = api.create_room(request.url, request.room, result.created, result.error);
            result.room = request.room;
            result.room.room_code = result.created.room_code;
            break;
        case RoomOperation::Attempt:
            if (!service_endpoints(api, request.url, result) ||
                !api.fetch_room(request.url, request.code, result.room, result.error)) break;
            if (!compatible(result.room)) { result.error = "Different Gauche build: update both games"; break; }
            result.okay = api.create_join_attempt(request.url, request.code, request.name, result.attempt, result.error);
            break;
        case RoomOperation::Finalize:
            result.okay = api.join_room(request.url, request.code, request.name, request.token, result.member, result.error);
            if (result.okay) api.fetch_room(request.url, request.code, result.room, result.error);
            break;
        case RoomOperation::Heartbeat:
            result.okay = api.heartbeat_room(request.url, request.code, request.member, request.name,
                request.secret, request.host ? &request.room : nullptr, result.error);
            if (result.okay) api.fetch_room(request.url, request.code, result.room, result.error);
            break;
        case RoomOperation::Leave:
            result.okay = api.leave_room(request.url, request.code, request.member, request.secret, result.error);
            break;
        }
    } catch (const std::exception&) {
        result.okay = false;
        result.error = "Malformed room service response";
    }
    if (api.last_clock.epoch_ms != 0) result.clock = api.last_clock;
    return result;
}
