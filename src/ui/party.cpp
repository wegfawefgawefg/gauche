#include "party.hpp"
#include "panel.hpp"
#include "text.hpp"

#include <cstdio>

const char* party_player_status(const Game& game, int owner) {
    if (owner < 0 || !has_player(game, owner)) return "OFFLINE";
    const PlayerId slot = owner;
    if (!player_state(game, slot).online) return "OFFLINE";
    const Entity* player = get_entity(game, player_state(game, slot).controlled);
    if (!player) return "JOINING";
    if (player->health <= 0) {
        if (game.run.death_policy == DeathPolicy::NextFloor) return "REVIVES NEXT FLOOR";
        if (game.run.death_policy == DeathPolicy::Entrance) return "RESPAWNING";
        return "DEAD";
    }
    if (game.run.phase == RunPhase::Reward)
        return player_state(game, slot).chosen ? "REWARD CHOSEN" : "CHOOSING REWARD";
    if (game.run.phase == RunPhase::Shop)
        return player_state(game, slot).shop_ready ? "READY FOR NEXT FLOOR" : "SHOPPING";
    if (game.run.phase == RunPhase::Won) return "FINISHED";
    if (player_state(game, slot).pending_count > 0) return "CHOOSING REWARD";
    return "EXPLORING";
}

void draw_party_status(SDL_Renderer* renderer, const Game& game, int local_owner,
                       float x, float y, bool horizontal) {
    int count = 0;
    for (const auto& [id, member] : game.players) if (member.online) ++count;
    if (count < 2) return;
    const int pages = (count+3)/4;
    const int page = static_cast<int>((SDL_GetTicks()/5000) % static_cast<std::uint64_t>(pages));
    if (pages>1) {
        char label[80]; std::snprintf(label,sizeof(label),"PARTY %d/%d - MORE IN F1 STATUS",page+1,pages);
        small_ui_text(renderer,x,y+ (horizontal ? 29 : 119),label);
    }
    int index = 0;
    for (const auto& [owner, participant] : game.players) {
        const PlayerId slot = owner;
        if (!player_state(game, slot).online) continue;
        const int display = index++;
        if (display/4 != page) continue;
        const int position = display%4;
        const float left = x + (horizontal ? static_cast<float>(position)*211 : 0);
        const float top = y + (horizontal ? 0 : static_cast<float>(position)*29);
        angled_panel(renderer, {left, top, 199, 26}, {66, 73, 69, 210});
        char label[32];
        std::snprintf(label, sizeof(label), "PLAYER %d%s", owner+1,
                      owner == local_owner ? " (YOU)" : "");
        small_ui_text(renderer, left+7, top+4, label, 220, 190, 136);
        small_ui_text(renderer, left+7, top+15, party_player_status(game, owner), 211, 218, 206);

    }
}
