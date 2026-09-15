#include "../items/muffling.hpp"
#include "../items/echo_pebble.hpp"
#include "playtest.hpp"
#include "../item_attribute.hpp"
#include "../artifacts/catalog.hpp"
#include "../items/storm_lantern.hpp"
#include "../props/candle.hpp"
#include "../items/kettle.hpp"
#include <imgui.h>
#include <algorithm>

namespace {

bool edit_item(Item& item) {
    bool changed = false;
    static ImGuiTextFilter search;
    if (ImGui::BeginCombo("Item", item_name(item.kind))) {
        search.Draw("Search");
        for (int index = 0; index < static_cast<int>(ItemKind::Count); ++index) {
            const auto kind = static_cast<ItemKind>(index);
            if (!search.PassFilter(item_name(kind))) continue;
            if (ImGui::Selectable(item_name(kind), item.kind == kind)) {
                item = make_item(kind); changed = true;
            }
        }
        ImGui::EndCombo();
    }
    if (item.kind == ItemKind::None) return changed;
    if (ImGui::BeginCombo("Attribute", item.attribute == ItemAttribute::None ? "None" : item_attribute_name(item.attribute))) {
        for (int index = 0; index <= static_cast<int>(ItemAttribute::Restorative); ++index) {
            const auto attribute = static_cast<ItemAttribute>(index);
            if (attribute != ItemAttribute::None && !item_accepts_attribute(item.kind, attribute)) continue;
            if (ImGui::Selectable(attribute == ItemAttribute::None ? "None" : item_attribute_name(attribute), item.attribute == attribute)) {
                item = make_item(item.kind, item.count, attribute); changed = true;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::TextWrapped("%s", item_attribute_effect(item.attribute));
    if (item.max_count > 1) changed |= ImGui::SliderInt("Stack", &item.count, 1, item.max_count);
    else ImGui::TextDisabled("Non-stackable");
    if (item.max_durability > 0) changed |= ImGui::SliderInt("Condition", &item.durability, 1, item.max_durability);
    if (item.max_uses > 0) changed |= ImGui::SliderInt("Uses remaining", &item.uses, 1, item.max_uses);
    if (item_is_gun(item.kind)) {
        changed |= ImGui::SliderInt(item.kind == ItemKind::Bow ? "Arrows" : "Loaded", &item.loaded, 0, make_item(item.kind).loaded);
        if (item.kind != ItemKind::Bow) changed |= ImGui::SliderInt("Reserve (this weapon)", &item.spare, 0, 999);
    }
    if (item.kind == ItemKind::StormLantern) {
        int seconds = (item.loaded+59)/60;
        if (ImGui::SliderInt("Fuel seconds", &seconds, 0, 120)) { item.loaded = seconds*60; changed = true; }
        changed |= ImGui::Checkbox("Shutter open", &item.opened);
        bool focused = item.light.shape == LightShape::Beam;
        if (ImGui::Checkbox("Focused beam", &focused)) {
            item.light.shape = focused ? LightShape::Beam : LightShape::Cone; changed = true;
        }
    }
    if (item.kind == ItemKind::EchoPebble) {
        const EchoVoice* voice = echo_voice(item);
        if (ImGui::BeginCombo("Recording",voice ? voice->name : "Blank (knock)")) {
            if (ImGui::Selectable("Blank (knock)",!voice)) { item.loaded = item.spare = 0; changed = true; }
            for (const EchoVoice& option : echo_voices)
                if (ImGui::Selectable(option.name,voice == &option)) {
                    item.loaded = static_cast<int>(option.sound)+1; item.spare = option.radius; changed = true;
                }
            ImGui::EndCombo();
        }
    }
    if (item.kind == ItemKind::CandleStub) {
        int seconds = (item.loaded+59)/60;
        if (ImGui::SliderInt("Candle fuel seconds", &seconds, 0, candle_fuel_ticks/60)) {
            item.loaded = seconds*60; changed = true;
        }
    }
    if (item.kind == ItemKind::SteamKettle) {
        constexpr const char* states[]{"Empty", "Cold water", "Boiling"};
        if (ImGui::Combo("Contents", &item.loaded, states, 3)) { item.spare = kettle_cool_ticks; changed = true; }
        if (item.loaded == 2) {
            int seconds = (item.spare+59)/60;
            if (ImGui::SliderInt("Heat seconds", &seconds, 1, kettle_cool_ticks/60)) { item.spare = seconds*60; changed = true; }
        }
    }
    if (item.kind == ItemKind::BearTrap) changed |= ImGui::Checkbox("Armed / open", &item.opened);
    if (item.kind == ItemKind::Stick) {
        int seconds = (item.flame_ticks+59)/60;
        if (ImGui::SliderInt("Burning seconds", &seconds, 0, 30)) { item.flame_ticks = seconds*60; changed = true; }
    }
    if (muffleable_item(item)) {
        int shots = item.muffled_uses;
        if (ImGui::SliderInt("Muffled shots", &shots, 0, 6)) { item.muffled_uses = static_cast<std::uint8_t>(shots); changed = true; }
    }
    return changed;
}
}

void draw_loadout_editor(const Game& game, bool offline) {
    auto& tools = playtest_tools();
    auto& kit = tools.loadout;
    ImGui::SetNextWindowPos({350, 50}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({550, 660}, ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Debug: Loadout", &tools.loadouts)) {
        bool changed = false;
        ImGui::TextWrapped("Edit a saved kit, then apply it now or use it on future spawns. Applying replaces equipment and restores kit HP.");
        constexpr const char* presets[]{"Forest entry", "Forest veteran", "Fire entry", "Ice entry", "Projectile lab"};
        ImGui::Combo("Preset", &tools.preset, presets, 5);
        if (ImGui::Button("Load preset into editor")) { set_loadout_preset(kit, tools.preset); changed = true; }
        const Entity* player = get_entity(game, game.players[0]);
        ImGui::SameLine();
        ImGui::BeginDisabled(!player);
        if (ImGui::Button("Copy current equipment")) {
            kit.inventory = player->inventory; kit.artifacts = player->artifacts;
            kit.health = player->max_health; kit.step_ticks = player->move_interval;
            kit.gold = game.run.coins[0]; changed = true;
        }
        ImGui::EndDisabled();
        ImGui::SeparatorText("Inventory slots");
        for (int slot = 0; slot < quick_slots; ++slot) {
            ImGui::PushID(slot);
            const std::string label = std::to_string(slot+1) + ": " + item_display_name(kit.inventory.slots[static_cast<std::size_t>(slot)]);
            if (ImGui::Selectable(label.c_str(), tools.selected_slot == slot)) tools.selected_slot = slot;
            ImGui::PopID();
        }
        ImGui::PushID(tools.selected_slot);
        changed |= edit_item(kit.inventory.slots[static_cast<std::size_t>(tools.selected_slot)]);
        ImGui::PopID();
        int held = kit.inventory.selected + 1;
        if (ImGui::SliderInt("Initially held slot", &held, 1, quick_slots)) { kit.inventory.selected = held-1; changed = true; }
        ImGui::SeparatorText("Player / artifacts");
        changed |= ImGui::SliderInt("Maximum HP", &kit.health, 1, 999);
        changed |= ImGui::SliderInt("Step interval (ticks)", &kit.step_ticks, 1, 60);
        ImGui::TextDisabled("Final step interval, including Fleet Feet; lower is faster.");
        changed |= ImGui::SliderInt("Gold", &kit.gold, 0, 9999);
        for (auto artifact : artifact_kinds) {
            const auto bit = 1U << static_cast<unsigned int>(artifact);
            bool enabled = (kit.artifacts & bit) != 0;
            if (ImGui::Checkbox(artifact_name(artifact), &enabled)) {
                if (enabled) kit.artifacts |= bit; else kit.artifacts &= ~bit;
                changed = true;
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", artifact_description(artifact));
        }
        ImGui::Separator();
        ImGui::BeginDisabled(!offline || !player || player->health <= 0);
        if (ImGui::Button("Apply to player now")) tools.equip_requested = true;
        ImGui::EndDisabled();
        changed |= ImGui::Checkbox("Use this as my spawn loadout", &tools.override_loadout);
        ImGui::TextWrapped("Saved across launches. Applies on new runs, debug jumps and respawns; surviving normal floor transitions keep earned loot.");
        if (!offline) ImGui::TextWrapped("Live changes and spawn overrides are offline-only.");
        if (changed) {
            for (Item& item : kit.inventory.slots) normalize_test_item(item);
            save_playtest_tools();
        }
        if (!tools.save_error.empty()) ImGui::TextWrapped("%s", tools.save_error.c_str());
    }
    ImGui::End();
}
