#include "inspection.hpp"
#include "../menu_shell.hpp"
#include "../menu/actions.hpp"
#include <emscripten.h>
#include <cstdlib>
EM_JS(char*, browser_command, (), {
    const command=Module.command || "";Module.command="";return stringToNewUTF8(command);
});
EM_JS(void, browser_state, (const char* json), {Module.gameState=JSON.parse(UTF8ToString(json));});
void browser_inspect(MenuShell& menu, float zoom) {
    menu.front.fullscreen=EM_ASM_INT({return !!Module.browserFullscreen;})!=0;
    menu.front.auto_reports=EM_ASM_INT({return !!Module.autoReports;})!=0;
    char* notice=reinterpret_cast<char*>(EM_ASM_PTR({const text=Module.browserNotice || "";Module.browserNotice="";return stringToNewUTF8(text);}));
    if(*notice)menu.front.toast=notice;
    std::free(notice);
    char* command=browser_command();
    if(*command)apply_menu_action(menu,command);
    std::free(command);
    static std::uint64_t last=0;
    const auto now=SDL_GetTicks();
    if(now-last<100)return;
    last=now;
    const auto frame=gubsy_get_frame(*menu.runtime);
    const auto& net=*menu.network;
    const auto& game=net.role==NetRole::Solo ? *menu.solo_game : net.rollback.game;
    nlohmann::json state={{"tick",game.tick},{"netLog",net.diagnostics.log_path},{"floor",game.run.floor},{"recoveries",net.diagnostics.recovery_count},{"host",net.role==NetRole::Host},{"rtt",net.round_trip_ms},{"confirmed",net.rollback.confirmed_through},{"started",game.started},{"over",game.game_over},
        {"owner",net.local_owner},{"ready",net.ready},{"room",menu.rooms.code},
        {"status",net.status},{"roomStatus",menu.front.room_status},
        {"players",game.players.size()},{"snapshot",net.last_snapshot_id},
        {"playing",menu.playing},{"menu",menu.visible},{"phase",static_cast<int>(game.run.phase)}};
    state["zoom"]=zoom;
    state["seed"]=std::to_string(game.run.seed);
    state["fullscreen"]=menu.front.fullscreen;
    state["autoReports"]=menu.front.auto_reports;
    state["vsync"]=menu.front.vsync;
    state["windowSize"]={frame.window_width,frame.window_height};
    state["renderSize"]={frame.render_width,frame.render_height};
    state["listedRooms"]=nlohmann::json::array();
    for(const auto& room:menu.front.rooms)state["listedRooms"].push_back(room.room_code);
    if(has_player(game,net.local_owner)) {
        if(const auto* player=get_entity(game,player_state(game,net.local_owner).controlled))
            state["position"]={player->cell.x,player->cell.y};
    }
    browser_state(state.dump().c_str());
}
