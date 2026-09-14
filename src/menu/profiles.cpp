#include "profiles.hpp"
#include "front_page.hpp"
#include "../input.hpp"
#include "src/engine_state.hpp"

#include <algorithm>
#include <optional>

// TUNING: Binds and controller settings travel together as one profile.
void ensure_input_pair(EngineState& engine, const BindsProfile& binds) {
    const auto existing = std::find_if(engine.input_settings_profiles.begin(),
        engine.input_settings_profiles.end(), [&](const InputSettingsProfile& tuning) {
            return tuning.id == binds.id;
        });
    if (existing != engine.input_settings_profiles.end()) return;
    InputSettingsProfile tuning{
        .id = binds.id, .name = binds.name,
        .mouse_sensitivity = 1.0F, .mouse_invert_x = false, .mouse_invert_y = false,
        .controller_sensitivity = 1.0F, .stick_deadzone = 0.15F,
        .trigger_threshold = 0.1F, .controller_invert_x = false,
        .controller_invert_y = false, .vibration_enabled = true,
        .vibration_strength = 1.0F,
    };
    engine.input_settings_profiles.push_back(tuning);
    (void)save_input_settings_profile(tuning);
}

bool default_profile(const BindsProfile& profile) {
    return profile.name == "DefaultBinds" || profile.name == "Default";
}

bool profile_read_only(const FrontPage& page) {
    const BindsProfile* profile = gubsy_find_binds_profile(*page.backend, page.selected_profile);
    return profile == nullptr || default_profile(*profile);
}

int active_profile_id(const FrontPage& page) {
    const auto& players = gubsy_get_lobby_state(*page.backend).local_players;
    if (page.selected_player < 0 || page.selected_player >= static_cast<int>(players.size()))
        return -1;
    return players[static_cast<std::size_t>(page.selected_player)].binds_profile_id;
}

// NAMES: Losing focus commits a valid name; invalid names retain the saved one.
bool save_profile_name(FrontPage& page) {
    const BindsProfile* source = gubsy_find_binds_profile(*page.backend, page.selected_profile);
    if (source == nullptr || default_profile(*source)) return true;
    const auto first = page.profile_name.find_first_not_of(" \t\r\n");
    const auto last = page.profile_name.find_last_not_of(" \t\r\n");
    const std::string name = first == std::string::npos ? "" :
        page.profile_name.substr(first, last - first + 1);
    if (name == source->name) { page.profile_name = name; return true; }
    for (const BindsProfile& other : gubsy_get_binds_profiles(*page.backend)) {
        if (other.id != source->id && other.name == name) {
            page.toast = "That profile name is already taken";
            page.profile_name = source->name;
            page.dirty = true;
            return false;
        }
    }
    if (name.empty() || name == "Default" || name == "DefaultBinds") {
        page.toast = "Choose a nonempty custom profile name";
        page.profile_name = source->name;
        page.dirty = true;
        return false;
    }
    BindsProfile edited = *source;
    edited.name = name;
    if (!gubsy_replace_binds_profile(*page.backend, edited)) {
        page.toast = "Could not save profile name";
        page.dirty = true;
        return false;
    }
    for (InputSettingsProfile& tuning : gubsy_runtime_engine(*page.backend).input_settings_profiles)
        if (tuning.id == edited.id) {
            tuning.name = name;
            (void)save_input_settings_profile(tuning);
        }
    page.profile_name = name;
    page.toast = "Name saved";
    page.dirty = true;
    return true;
}

namespace {

void use_profile(FrontPage& page, int id) {
    const BindsProfile* profile = gubsy_find_binds_profile(*page.backend, id);
    if (profile == nullptr) return;
    ensure_input_pair(gubsy_runtime_engine(*page.backend), *profile);
    const bool binds = gubsy_set_lobby_player_binds_profile(*page.backend,
        page.selected_player, id);
    const bool tuning = gubsy_set_lobby_player_input_settings_profile(*page.backend,
        page.selected_player, id);
    page.toast = binds && tuning ? "Using " + profile->name + " for Player " +
        std::to_string(page.selected_player + 1) : "Could not select profile";
    page.dirty = true;
}

void create_profile(FrontPage& page, bool copy) {
    BindsProfile created = default_game_binds();
    std::optional<InputSettingsProfile> source_tuning;
    EngineState& engine = gubsy_runtime_engine(*page.backend);
    if (copy) {
        const BindsProfile* source = gubsy_find_binds_profile(*page.backend, page.selected_profile);
        if (source == nullptr) return;
        created = *source;
        for (const InputSettingsProfile& tuning : engine.input_settings_profiles)
            if (tuning.id == source->id) source_tuning = tuning;
    }
    created.id = 1;
    for (const BindsProfile& existing : gubsy_get_binds_profiles(*page.backend))
        created.id = std::max(created.id, existing.id + 1);
    const std::string base = copy && !default_profile(created) ? created.name + " copy " : "Custom ";
    int suffix = created.id;
    do {
        created.name = base + std::to_string(suffix++);
    } while (std::any_of(engine.binds_profiles.begin(), engine.binds_profiles.end(),
        [&](const BindsProfile& other) { return other.name == created.name; }));
    if (!gubsy_replace_binds_profile(*page.backend, created)) {
        page.toast = "Could not create profile"; page.dirty = true; return;
    }
    if (source_tuning) {
        source_tuning->id = created.id;
        source_tuning->name = created.name;
        engine.input_settings_profiles.push_back(*source_tuning);
        (void)save_input_settings_profile(*source_tuning);
    } else ensure_input_pair(engine, created);
    page.selected_profile = created.id;
    page.profile_name = created.name;
    show_menu_screen(page, MenuScreen::ProfileEditor);
    use_profile(page, created.id);
}

} // namespace

// PROFILE ACTIONS: The list selects a profile; its separate Edit button opens it.
bool profile_action(FrontPage& page, std::string_view action) {
    if (!action.starts_with("profile:")) return false;
    if (action == "profile:bindings") show_menu_screen(page, MenuScreen::Bindings);
    else if (action == "profile:new" || action == "profile:copy")
        create_profile(page, action == "profile:copy");
    else if (action.starts_with("profile:use:"))
        use_profile(page, std::stoi(std::string{action.substr(12)}));
    else {
        page.selected_profile = std::stoi(std::string{action.substr(8)});
        const BindsProfile* selected = gubsy_find_binds_profile(*page.backend, page.selected_profile);
        if (selected == nullptr) return true;
        page.profile_name = selected->name;
        show_menu_screen(page, MenuScreen::ProfileEditor);
    }
    return true;
}
