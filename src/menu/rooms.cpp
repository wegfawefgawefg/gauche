#include "rooms.hpp"
#include "front_page.hpp"
#include "page_chrome.hpp"

#include <algorithm>
#include <bit>

using namespace gauche_menu;

namespace {
void player_name_row(ViewBuilder& ui) {
    ui.layout_container("card", "name-row", glayout::ContainerKind::Row,
        {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Pixels, 44}, 12);
    ui.text_input("name-row", "player-name", "Your name", "player-name", "menu", 44);
    ui.layout("player-name").size.width = {glayout::LengthKind::Fill, 1};
    ui.button("name-row", "shuffle-name", "Shuffle", "room:shuffle-name", "menu", 44);
    ui.layout("shuffle-name").size.width = {glayout::LengthKind::Pixels, 128 * ui.scale()};
}
}

void online_host_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Host Game", 720, 465);
    ui.label("card", "host-help", "Friends can find your game in Join Game or use its room code.", 46, 17);
    player_name_row(ui);
    ui.text_input("card", "room-name", "Room name", "room-name", "menu", 46);
    button(ui, "room-create", page.room_busy ? "Creating room…" : "Host Game", "room:create", 52);
    ui.label("card", "room-status", page.room_status, 40, 15);
    footer(ui, "room-create");
}

void rooms_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Join Game", 850, 690);
    player_name_row(ui);
    ui.layout_container("card", "join-code-row", glayout::ContainerKind::Row,
        {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Pixels, 44}, 12);
    ui.text_input("join-code-row", "room-code", "Room code", "room-code", "menu", 44);
    ui.layout("room-code").size.width = {glayout::LengthKind::Fill, 1};
    ui.button("join-code-row", "room-join", "Join by Code", "room:join", "menu", 44);
    ui.layout("room-join").size.width = {glayout::LengthKind::Pixels, 190 * ui.scale()};
    ui.label("card", "browser-heading", "Public Games", 30, 22);
    ui.layout_container("card", "room-list", glayout::ContainerKind::Column,
        {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Fill, 1}, 10);
    ui.scrolling("room-list");
    if (page.rooms.empty()) ui.label("room-list", "empty-rooms",
        page.room_busy ? "Looking for games…" : "No public games yet. Ask your friend to host one.", 54, 17);
    for (const auto& room : page.rooms) {
        const std::string id = "listed-room-" + room.room_code;
        ui.button("room-list", id, room.session_name + "  ·  " + room.host_name + "  ·  " +
            std::to_string(room.current_players) + "/" + std::to_string(room.max_players) + " players",
            "room:code:" + room.room_code, "menu", 54);
        ui.spec(id).text_style.size = 19 * ui.scale();
    }
    button(ui, "room-refresh", "Refresh", "room:browse", 36);
    ui.label("card", "room-status", page.room_status, 38, 15);
    footer(ui, page.rooms.empty() ? "room-code" : "listed-room-" + page.rooms.front().room_code);
}

void network_options_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Advanced Connection Options", 750, 470);
    ui.label("card", "advanced-help", "Normal hosting and joining handle connections automatically.", 44, 17);
#ifdef __EMSCRIPTEN__
    (void)page;
    ui.label("card", "browser-relay", "Browser games connect through the Teeming relay.", 50, 18);
    footer(ui, "back");
#else
    ui.text_input("card", "room-url", "Room service", "room-url", "menu", 44);
    button(ui, "room-relay", std::string("Connection  ·  ") +
        (page.force_relay ? "Force relay" : "Automatic"), "room:relay", 42);
    button(ui, "direct-host", "Host by IP / LAN", "direct-host", 42);
    button(ui, "direct-join", "Join by IP / LAN", "direct-join", 42);
    footer(ui, "room-url");
#endif
}

void party_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Party  ·  " + page.party_code, 820, 620);
    ui.label("card", "party-code", "Share this room code:  " + page.party_code, 52, 24);
    ui.label("card", "party-transport", page.connection_status, 30, 16);
    for (std::size_t i = 0; i < page.room_members.size(); ++i) {
        const auto& member = page.room_members[i];
        ui.label("card", "party-member-" + std::to_string(i), member.display_name +
            (member.is_host ? "  ·  Host" : ""), 34, 19);
    }
    ui.label("card", "ready-count", std::to_string(page.party_ready_count) +
        " players ready", 34, 17);
    button(ui, "party-ready", page.party_ready ? "Ready  ·  Click to unready" : "Ready Up", "room:ready");
    if (page.party_host) button(ui, "party-start", "Start Run", "room:start");
    ui.label("card", "party-status", page.room_status, 52, 16);
    button(ui, "party-leave", "Leave Room", "room:leave", 40);
    ui.focus_group("menu", "party-ready", "card");
}
