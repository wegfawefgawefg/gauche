#include "playtest.hpp"
#include "levels.hpp"
#include <imgui.h>

void draw_level_editor(bool offline) {
    auto& tools = playtest_tools();
    ImGui::SetNextWindowPos({340, 40}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({470, 640}, ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Debug: Levels", &tools.levels)) {
        ImGui::TextWrapped("Jump to a stage. Generated stages roll fresh layouts; uniques use their authored layout.");
        if (ImGui::BeginListBox("##levels", {-1, 230})) {
            for (int index = 0; index < static_cast<int>(test_levels.size()); ++index) {
                const auto& level = test_levels[static_cast<std::size_t>(index)];
                if (ImGui::Selectable(level.name, tools.selected_level == index)) {
                    tools.selected_level = index; save_playtest_tools();
                }
            }
            ImGui::EndListBox();
        }
        ImGui::BeginDisabled(!offline);
        if (ImGui::Button("Jump / restart selected stage")) tools.jump_requested = true;
        ImGui::EndDisabled();
        if (!offline) ImGui::TextWrapped("Live changes are offline-only. Saved overrides do not affect network games.");
        ImGui::SeparatorText("New runs");
        if (ImGui::Checkbox("Override starting stage", &tools.override_start)) {
            if (tools.override_start) tools.starting_level = tools.selected_level;
            save_playtest_tools();
        }
        ImGui::Text("Start: %s", tools.override_start ? test_levels[static_cast<std::size_t>(tools.starting_level)].name : "Normal run");
        if (ImGui::Button("Use selection as starting stage")) {
            tools.starting_level = tools.selected_level; tools.override_start = true; save_playtest_tools();
        }
        ImGui::TextWrapped("Saved across launches. Progression continues normally unless Repeat is enabled.");
        ImGui::SeparatorText("After clearing a stage");
        if (ImGui::Checkbox("Repeat selected stage", &tools.repeat)) {
            if (tools.repeat) tools.repeat_level = tools.selected_level;
            save_playtest_tools();
        }
        ImGui::Text("Repeat: %s", tools.repeat ? test_levels[static_cast<std::size_t>(tools.repeat_level)].name : "Off");
        if (ImGui::Button("Use selection as repeat target")) {
            tools.repeat_level = tools.selected_level; tools.repeat = true; save_playtest_tools();
        }
        ImGui::TextWrapped("Repeats after rewards and any shop. Change the start override separately for death/restart tests.");
        if (ImGui::Button("Disable all playtest overrides")) {
            tools.override_start = tools.repeat = tools.override_loadout = false; save_playtest_tools();
        }
        ImGui::TextDisabled("Bear dens are room variants, not a standalone unique yet.");
        if (!tools.save_error.empty()) ImGui::TextWrapped("%s", tools.save_error.c_str());
    }
    ImGui::End();
}

void draw_playtest_tools(const Game& game, bool offline) {
    auto& tools = playtest_tools();
    if (tools.levels) draw_level_editor(offline);
    if (tools.loadouts) draw_loadout_editor(game, offline);
}
