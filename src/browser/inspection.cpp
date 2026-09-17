#include "inspection.hpp"
#include "../menu_shell.hpp"
#include "../menu/actions.hpp"
#include <emscripten.h>
#include <cstdlib>
EM_JS(char*, browser_command, (), {
    const command=Module.command || "";Module.command="";return stringToNewUTF8(command);
});
EM_JS(void, browser_state, (const char* json), {Module.gameState=JSON.parse(UTF8ToString(json));});
void browser_inspect(MenuShell& menu) {
    char* command=browser_command();
    if(*command)apply_menu_action(menu,command);
    std::free(command);
    static std::uint64_t last=0;
    const auto now=SDL_GetTicks();
    if(now-last<100)return;
    last=now;
    const auto& net=*menu.network;
    const auto& game=net.role==NetRole::Solo ? *menu.solo_game : net.rollback.game;
    nlohmann::json state={{"tick",game.tick},{"netLog",net.diagnostics.log_path},{"floor",game.run.floor},{"recoveries",net.diagnostics.recovery_count},{"host",net.role==NetRole::Host},{"rtt",net.round_trip_ms},{"confirmed",net.rollback.confirmed_through},{"started",game.started},{"over",game.game_over},
        {"owner",net.local_owner},{"ready",net.ready},{"room",menu.rooms.code},
        {"status",net.status},{"roomStatus",menu.front.room_status},
        {"players",game.players.size()},{"snapshot",net.last_snapshot_id},
        {"playing",menu.playing},{"menu",menu.visible},{"phase",static_cast<int>(game.run.phase)}};
    state["listedRooms"]=nlohmann::json::array();
    for(const auto& room:menu.front.rooms)state["listedRooms"].push_back(room.room_code);
    if(has_player(game,net.local_owner)) {
        if(const auto* player=get_entity(game,player_state(game,net.local_owner).controlled))
            state["position"]={player->cell.x,player->cell.y};
    }
    browser_state(state.dump().c_str());
}
