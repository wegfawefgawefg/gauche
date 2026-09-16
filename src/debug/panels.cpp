#include "panels.hpp"
#include "worldgen.hpp"
#include "playtest.hpp"
#include "../input.hpp"

#include <imgui.h>
#include "src/imgui_layer.hpp"

namespace {

DebugPanels panels;

} // namespace

DebugPanels& debug_panels() { return panels; }

void init_debug_panels(SDL_Window* window, SDL_Renderer* renderer) {
    if (!init_imgui_layer(window, renderer)) return;
    // INPUT: Gamepads remain owned by Gubsy's binding profiles.
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
    // CURSOR: The SDL backend must not re-show the OS cursor during NewFrame.
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
}

void shutdown_debug_panels() { shutdown_imgui_layer(); }

bool debug_event(const SDL_Event& event) {
    if (!GAUCHE_DEV_MODE) return false;
    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
        if (event.key.key == SDLK_F1) { panels.visible = !panels.visible; return true; }
        if (event.key.key == SDLK_F2 && panels.visible) { panels.selector = !panels.selector; return true; }
    }
    const bool keyboard = event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP ||
                          event.type == SDL_EVENT_TEXT_INPUT;
    const bool mouse = event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP ||
                       event.type == SDL_EVENT_MOUSE_WHEEL || event.type == SDL_EVENT_MOUSE_MOTION;
    return panels.visible && ((keyboard && imgui_want_capture_keyboard()) ||
                              (mouse && imgui_want_capture_mouse()));
}

bool debug_captures_input() {
    return panels.visible && (imgui_want_capture_mouse() || imgui_want_capture_keyboard());
}

void draw_debug_panels(const Game& game, int owner, bool offline) {
    if (!imgui_is_initialized()) return;
    ImGui::GetIO().MouseDrawCursor = false;
    imgui_new_frame();
    if (panels.visible && panels.selector) {
        ImGui::SetNextWindowPos({16, 16}, ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Gauche Debug [F2]", &panels.selector, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextUnformatted("F1: hide/show all windows   F2: selector");
            if (ImGui::CollapsingHeader("Gameplay", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Generation inspector", &worldgen_viewer().details);
                ImGui::Checkbox("Combat overlays", &panels.combat);
                ImGui::Checkbox("Player status", &panels.status);
                ImGui::Checkbox("Levels / start override", &playtest_tools().levels);
                ImGui::Checkbox("Loadout / spawn override", &playtest_tools().loadouts);
                ImGui::Checkbox("Pause offline world while F1 is open", &playtest_tools().pause);
            }
            if (ImGui::CollapsingHeader("Presentation", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Contact shadows", &panels.contact_shadows);
                ImGui::Checkbox("Creature / item shadows", &panels.shadow_entities);
                ImGui::Checkbox("Prop shadows", &panels.shadow_props);
                ImGui::Checkbox("Loose debris shadows", &panels.shadow_debris);
                ImGui::TextUnformatted("Ground anchors only; independent of lights.");
            }
        }
        ImGui::End();
    }
    if (panels.visible && panels.combat) {
        ImGui::SetNextWindowPos({16, 180}, ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Debug: Combat", &panels.combat, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Checkbox("Held item pattern in world", &panels.world_items);
            ImGui::Checkbox("Enemy attack patterns in world", &panels.world_enemies);
            ImGui::TextUnformatted("Inventory diagrams are always available.");
            ImGui::TextUnformatted("These overlays only change local presentation.");
        }
        ImGui::End();
    }
    if (panels.visible && panels.status) {
        ImGui::SetNextWindowPos({16, 340}, ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Debug: Player status", &panels.status, ImGuiWindowFlags_AlwaysAutoResize)) {
            const Entity* player = owner >= 0 && owner < 4 ? get_entity(game, game.players[static_cast<std::size_t>(owner)]) : nullptr;
            if (player != nullptr) {
                ImGui::Text("HP %d/%d | cell %d,%d", player->health, player->max_health, player->cell.x, player->cell.y);
                ImGui::Text("Scorch %d | burn %d | sleep %d | stun %d | chill %d ticks",
                    player->scorch_ticks, player->burn_ticks, player->sleep_ticks, player->stun_ticks, player->freeze_ticks);
            }
        }
        ImGui::End();
    }
    if (panels.visible) {
        draw_playtest_tools(game, offline && !worldgen_viewer().active);
        if (worldgen_viewer().details) draw_worldgen_details();
    }
    imgui_render_layer();
}
