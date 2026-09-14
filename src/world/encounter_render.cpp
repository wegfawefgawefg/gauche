#include "encounter.hpp"
#include "../ui/scale.hpp"
#include "../ui/text.hpp"

#include <cstdio>
#include <cstring>

void draw_encounter_status(SDL_Renderer* renderer, const Game& game, const Entity& player) {
    for (const Entity& controller : game.entities) {
        if (controller.kind != EntityKind::Encounter || distance(player.cell, controller.cell) > 60) continue;
        const auto phase = static_cast<EncounterPhase>(controller.label_a);
        char text[120]{};
        switch (phase) {
        case EncounterPhase::Dormant:
            if (distance(player.cell, controller.cell) > 4) return;
            std::snprintf(text, sizeof(text), "USE LEVER: SEAL THE HOUSE. SURVIVE %d WAVES.", controller.counter_a);
            break;
        case EncounterPhase::Gathering:
            std::snprintf(text, sizeof(text), "MANSION: GATHER INSIDE AND CLEAR THE GATES");
            break;
        case EncounterPhase::Countdown:
            std::snprintf(text, sizeof(text), "GATES CLOSING IN %.1f", static_cast<double>(controller.timer_a) / 60.0);
            break;
        case EncounterPhase::Fighting: {
            int remaining = controller.counter_b;
            const Handle handle{static_cast<int>(&controller - game.entities.data()), controller.generation};
            for (const Entity& enemy : game.entities)
                if (enemy.kind != EntityKind::None && enemy.encounter == handle && enemy.health > 0)
                    remaining += enemy.kind == EntityKind::ZombieStack ? enemy.counter_a : 1;
            std::snprintf(text, sizeof(text), "MANSION WAVE %d/%d   %d REMAIN", controller.label_b,
                          controller.counter_a, remaining);
            break;
        }
        case EncounterPhase::Intermission:
            std::snprintf(text, sizeof(text), "RELOAD. SUPPLIES BY THE LEVER. NEXT WAVE IN %.1f",
                          static_cast<double>(controller.timer_a) / 60.0);
            break;
        case EncounterPhase::Released:
            std::snprintf(text, sizeof(text), "THE HOUSE IS QUIET. GATES OPEN. FIND THE EXIT.");
            break;
        }
        const HudScale scale{renderer};
        const float width = static_cast<float>(std::strlen(text)) * 4;
        small_ui_text(renderer, (640.0F / ui_scale - width) * .5F, 336.0F / ui_scale,
                      text, 209, 183, 222);
        return;
    }
}
