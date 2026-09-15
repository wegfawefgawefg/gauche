#include "status.hpp"
#include "text.hpp"

#include <algorithm>
#include <array>
#include <cstdio>

namespace {

struct StatusRow {
    const char* name;
    int ticks;
    SDL_Color color;
    const char* effect;
    Sprite icon;
};

} // namespace

void draw_player_status(SDL_Renderer* renderer, const GameGraphics& graphics,
                         const Game& game, const Entity& player, float x, float bottom) {
    if (player.health <= 0) return;
    char burning[80];
    const int weak = static_cast<int>((game.tick + static_cast<std::uint64_t>(player.scorch_ticks)) / 30 - game.tick / 30) * 2;
    const int strong = static_cast<int>((game.tick + static_cast<std::uint64_t>(player.burn_ticks)) / 30 - game.tick / 30) * 4;
    const int per_second = (player.scorch_ticks > 0 ? 4 : 0) + (player.burn_ticks > 0 ? 8 : 0);
    std::snprintf(burning, sizeof(burning), "%d HP/S NOW | %d DMG LEFT", per_second, weak + strong);
    char healing[64], speed[64];
    const int healing_ticks = player.vitals.healing_left == 0 ? 0 :
        (player.vitals.healing_left - 1) * recovery_interval(player.vitals) + player.vitals.healing_wait;
    std::snprintf(healing, sizeof(healing), "%d HP/S | %d LEFT%s",
        60 / recovery_interval(player.vitals), player.vitals.healing_left,
        player.vitals.recovery == RecoveryKind::Broth ? " | HIT ENDS" : "");
    const Sprite recovery_icon = player.vitals.recovery == RecoveryKind::Broth ? Sprite::HotBroth :
        player.vitals.recovery == RecoveryKind::Poultice ? Sprite::IcePoultice : Sprite::HerbBag;
    std::snprintf(speed, sizeof(speed), "STEP %d TICKS | BURN AFTER", movement_beat(player, player.move_interval));
    const std::array rows{
        StatusRow{"BURNING", std::max(player.scorch_ticks, player.burn_ticks),
                  {233, 150, 76, 255}, burning, Sprite::FlameA},
        StatusRow{"NAUSEOUS", player.vitals.nausea, {173, 192, 105, 255},
                  "1 HP/S | WATER CLEARS", Sprite::StatusNausea},
        StatusRow{"ASLEEP", player.sleep_ticks, {184, 164, 224, 255},
                  "DAMAGE/BITTER ROOT WAKES", Sprite::StatusSleep},
        StatusRow{"STUNNED", player.stun_ticks, {231, 198, 91, 255},
                  "NO ACTIONS | SPLINT CURES", Sprite::StatusStun},
        StatusRow{"ROOTED", player.vitals.rooted, {209, 185, 131, 255},
                  "NO STEPS | CAN STILL ATTACK", player.vitals.root_kind == RootKind::Net ? Sprite::ThrowingNet : Sprite::RopeSnare},
        StatusRow{"GRIPPING", player.vitals.grip, {177, 191, 115, 255},
                  "NO SHOVE/SLIP | SLOW STEPS", Sprite::StickyBoots},
        StatusRow{"CRAMPONS", player.vitals.traction, {156,195,211,255},
                  "NO ICE SLIP | SLOW STEPS", Sprite::Crampons},
        StatusRow{"SLIDE REACH", player.inventory.held()->kind==ItemKind::SkateBlade ? player.vitals.slide_momentum : 0,
                  {179,212,225,255}, "ONE EXTRA FORWARD CELL", Sprite::SkateBlade},
        StatusRow{"CHILLED", player.freeze_ticks, {121, 191, 230, 255},
                  "STEP RECOVERY HALF SPEED", Sprite::StatusChill},
        StatusRow{"REGENERATING", healing_ticks, {159, 200, 118, 255}, healing, recovery_icon},
        StatusRow{"INSULATED", player.vitals.chill_guard, {188, 210, 211, 255},
                  "RESISTS CHILL | FLAMMABLE", Sprite::WoolWrap},
        StatusRow{"WAKEFUL", player.vitals.sleep_guard, {190, 159, 113, 255},
                  "RESISTS SLEEP", Sprite::BitterRoot},
        StatusRow{"BRACED", player.vitals.stun_guard, {210, 192, 137, 255},
                  "RESISTS STUN", Sprite::Splint},
        StatusRow{"CHILI RUSH", player.vitals.haste, {229, 126, 83, 255}, speed, Sprite::Chili},
    };
    for (const StatusRow& row : rows) {
        if (row.ticks <= 0) continue;
        bottom -= 25;
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_FRect panel{x, bottom, 201, 23};
        SDL_SetRenderDrawColor(renderer, 16, 21, 22, 225);
        SDL_RenderFillRect(renderer, &panel);
        SDL_FRect stripe{x, bottom, 2, 23};
        SDL_SetRenderDrawColor(renderer, row.color.r, row.color.g, row.color.b, 235);
        SDL_RenderFillRect(renderer, &stripe);
        SDL_FRect icon{x + 4, bottom + 4, 13, 13};
        SDL_RenderTexture(renderer, texture_for(graphics, row.icon), nullptr, &icon);
        char label[64];
        std::snprintf(label, sizeof(label), "%s  %.1fs", row.name, static_cast<double>(row.ticks) / 60.0);
        small_ui_text(renderer, x + 20, bottom + 3, label, row.color.r, row.color.g, row.color.b);
        small_ui_text(renderer, x + 20, bottom + 13, row.effect, 187, 190, 179);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
}
