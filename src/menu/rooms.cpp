#include "rooms.hpp"
#include "front_page.hpp"
#include "page_chrome.hpp"

#include <algorithm>
#include <bit>

using namespace gauche_menu;

void rooms_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Online Rooms", 1080, 700);
    ui.layout_container("card", "rooms-body", glayout::ContainerKind::Row,
        {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Fill, 1}, 24);
    for (const char* pane : {"room-connect", "room-browser"}) {
        ui.layout_container("rooms-body", pane, glayout::ContainerKind::Column,
            {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Fill, 1}, 8);
        ui.scrolling(pane);
    }
    const auto action = [&](const char* pane, std::string id, std::string label,
                            std::string command, float height = 42) {
        ui.button(pane, id, std::move(label), std::move(command), "menu", height);
        ui.spec(id).text_style.size = 19 * ui.scale();
    };
    ui.text_input("room-connect", "player-name", "Your name", "player-name", "menu", 40);
    ui.text_input("room-connect", "room-name", "New room name", "room-name", "menu", 40);
    action("room-connect", "room-create", "Host a New Room", "room:create");
    ui.text_input("room-connect", "room-code", "Six-character room code", "room-code", "menu", 40);
    action("room-connect", "room-join", "Join by Code", "room:join");
    ui.text_input("room-connect", "room-url", "Room service", "room-url", "menu", 40);
    action("room-connect", "room-relay", std::string("Connection  ·  ") +
        (page.force_relay ? "Force relay" : "Automatic"), "room:relay", 36);
    action("room-browser", "room-refresh", "Refresh Public Rooms", "room:browse");
    const int first = page.room_page * 4;
    for (int i = first; i < std::min(first + 4, static_cast<int>(page.rooms.size())); ++i) {
        const auto& room = page.rooms[static_cast<std::size_t>(i)];
        action("room-browser", "listed-room-" + room.room_code,
            room.session_name + "  ·  " + room.host_name + "  ·  " +
            std::to_string(room.current_players) + "/" + std::to_string(room.max_players) +
            "  ·  " + room.room_code, "room:code:" + room.room_code, 58);
    }
    if (page.rooms.size() > 4) {
        action("room-browser", "rooms-previous", "Previous rooms", "room:previous", 32);
        action("room-browser", "rooms-next", "More rooms", "room:next", 32);
    }
    ui.label("card", "room-status", page.room_status, 42, 15);
    footer(ui, "room-create");
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
    ui.label("card", "ready-count", std::to_string(std::popcount(page.party_ready_mask)) +
        " players ready", 34, 17);
    button(ui, "party-ready", page.party_ready ? "Ready  ·  Click to unready" : "Ready Up", "room:ready");
    if (page.party_host) button(ui, "party-start", "Start Run", "room:start");
    ui.label("card", "party-status", page.room_status, 52, 16);
    button(ui, "party-leave", "Leave Room", "room:leave", 40);
    ui.focus_group("menu", "party-ready", "card");
}
