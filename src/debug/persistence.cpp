#include "playtest.hpp"
#include "../item_attribute.hpp"
#include "../artifacts/catalog.hpp"
#include <fstream>
#include <algorithm>

namespace { PlaytestTools state; }
PlaytestTools& playtest_tools() { return state; }

void init_playtest_tools(const std::filesystem::path& path) {
    state.path = path;
    set_loadout_preset(state.loadout, 0);
    std::ifstream input(path);
    if (!input) return;
    int version = 0;
    PlaytestTools loaded = state;
    auto& kit = loaded.loadout;
    input >> version >> loaded.selected_level >> loaded.override_start >> loaded.starting_level
          >> loaded.repeat >> loaded.repeat_level >> loaded.override_loadout
          >> kit.health >> kit.step_ticks >> kit.gold >> kit.artifacts >> kit.inventory.selected;
    for (Item& item : kit.inventory.slots) {
        int kind = 0, attribute = 0, shape = 0, muffled = 0;
        input >> kind >> attribute >> item.count >> item.durability >> item.uses
              >> item.loaded >> item.spare >> item.opened >> item.flame_ticks >> shape >> muffled;
        if (kind < 0 || kind >= static_cast<int>(ItemKind::Count) || attribute < 0 ||
            attribute > static_cast<int>(ItemAttribute::Restorative)) { input.setstate(std::ios::failbit); break; }
        item.kind = static_cast<ItemKind>(kind); item.attribute = static_cast<ItemAttribute>(attribute);
        item.light.shape = shape == 2 ? LightShape::Beam : LightShape::Cone;
        item.muffled_uses = static_cast<std::uint8_t>(std::clamp(muffled, 0, 8));
        normalize_test_item(item);
    }
    if (!input || version != 1) { state.save_error = "Could not read saved playtest settings; defaults are active."; return; }
    loaded.selected_level = std::clamp(loaded.selected_level, 0, 12);
    loaded.starting_level = std::clamp(loaded.starting_level, 0, 12);
    loaded.repeat_level = std::clamp(loaded.repeat_level, 0, 12);
    kit.health = std::clamp(kit.health, 1, 999); kit.step_ticks = std::clamp(kit.step_ticks, 1, 60);
    kit.gold = std::clamp(kit.gold, 0, 9999); kit.inventory.selected = std::clamp(kit.inventory.selected, 0, quick_slots-1);
    std::uint32_t allowed = 0;
    for (auto artifact : artifact_kinds) allowed |= 1U << static_cast<unsigned int>(artifact);
    kit.artifacts &= allowed;
    state = loaded;
}

// SAVE: A failed write leaves the previous configuration intact and reports it in ImGui.
void save_playtest_tools() {
    if (state.path.empty()) return;
    std::error_code error;
    std::filesystem::create_directories(state.path.parent_path(), error);
    auto temporary = state.path; temporary += ".tmp";
    std::ofstream out(temporary);
    const auto& kit = state.loadout;
    out << 1 << ' ' << state.selected_level << ' ' << state.override_start << ' ' << state.starting_level
        << ' ' << state.repeat << ' ' << state.repeat_level << ' ' << state.override_loadout
        << ' ' << kit.health << ' ' << kit.step_ticks << ' ' << kit.gold << ' ' << kit.artifacts
        << ' ' << kit.inventory.selected << '\n';
    for (const Item& item : kit.inventory.slots)
        out << static_cast<int>(item.kind) << ' ' << static_cast<int>(item.attribute) << ' '
            << item.count << ' ' << item.durability << ' ' << item.uses << ' ' << item.loaded << ' '
            << item.spare << ' ' << item.opened << ' ' << item.flame_ticks << ' '
            << static_cast<int>(item.light.shape) << ' ' << static_cast<int>(item.muffled_uses) << '\n';
    out.close();
    if (out) std::filesystem::rename(temporary, state.path, error);
    state.save_error = !out || error ? "Could not save playtest settings." : "";
}
