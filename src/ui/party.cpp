#include "party.hpp"
#include "panel.hpp"
#include "text.hpp"

#include <cstdio>

const char* party_player_status(const Game& game, int owner) {
    if (owner < 0 || owner >= 4) return "OFFLINE";
    const auto slot = static_cast<std::size_t>(owner);
    if (!game.run.online[slot]) return "OFFLINE";
    const Entity* player = get_entity(game, game.players[slot]);
    if (!player) return "JOINING";
    if (player->health <= 0) {
        if (game.run.death_policy == DeathPolicy::NextFloor) return "REVIVES NEXT FLOOR";
        if (game.run.death_policy == DeathPolicy::Entrance) return "RESPAWNING";
        return "DEAD";
    }
    if (game.run.phase == RunPhase::Reward)
        return game.run.chosen[slot] ? "REWARD CHOSEN" : "CHOOSING REWARD";
    if (game.run.phase == RunPhase::Shop)
        return game.run.shop_ready[slot] ? "READY FOR NEXT FLOOR" : "SHOPPING";
    if (game.run.phase == RunPhase::Won) return "FINISHED";
    if (game.run.pending_count[slot] > 0) return "CHOOSING REWARD";
    return "EXPLORING";
}

void draw_party_status(SDL_Renderer* renderer, const Game& game, int local_owner,
                       float x, float y, bool horizontal) {
    int count = 0;
    for (bool online : game.run.online) if (online) ++count;
    if (count < 2) return;
    int index = 0;
    for (int owner = 0; owner < 4; ++owner) {
        const auto slot = static_cast<std::size_t>(owner);
        if (!game.run.online[slot]) continue;
        const float left = x + (horizontal ? static_cast<float>(index)*211 : 0);
        const float top = y + (horizontal ? 0 : static_cast<float>(index)*29);
        angled_panel(renderer, {left, top, 199, 26}, {66, 73, 69, 210});
        char label[32];
        std::snprintf(label, sizeof(label), "PLAYER %d%s", owner+1,
                      owner == local_owner ? " (YOU)" : "");
        small_ui_text(renderer, left+7, top+4, label, 220, 190, 136);
        small_ui_text(renderer, left+7, top+15, party_player_status(game, owner), 211, 218, 206);
        ++index;
    }
}
