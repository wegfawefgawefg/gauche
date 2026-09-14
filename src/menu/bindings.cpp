#include "profiles.hpp"
#include "front_page.hpp"

// MAPPINGS: Changes are saved to the chosen custom profile immediately.
bool binding_action(FrontPage& page, std::string_view action) {
    if (action.starts_with("bind:")) {
        const bool read_only = profile_read_only(page);
        if (read_only && (action == "bind:add" || action == "bind:clear" ||
            action.starts_with("bind:remove:") || action.starts_with("bind:choice:"))) {
            page.toast = "Duplicate Defaults to change its bindings";
            page.dirty = true;
            return true;
        }
        if (action == "bind:add") {
            if (page.selected_bind_type == BindsActionType::Button) {
                page.capturing_bind = true;
                page.toast = "Press a key, mouse button, or gamepad button";
                page.dirty = true;
            } else show_menu_screen(page, MenuScreen::BindChoices);
            return true;
        }
        const BindsProfile* source = gubsy_find_binds_profile(*page.backend,
                                                               page.selected_profile);
        if (action == "bind:clear" || action.starts_with("bind:remove:") ||
            action.starts_with("bind:choice:")) {
            if (source == nullptr) return true;
            BindsProfile edited = *source;
            const bool clear = action == "bind:clear";
            const int mapping_index = action.starts_with("bind:remove:") ?
                std::stoi(std::string{action.substr(12)}) : -1;
            if (action.starts_with("bind:choice:")) {
                const int choice = std::stoi(std::string{action.substr(12)});
                const auto& options = binds_input_choices(page.selected_bind_type);
                if (choice < 0 || choice >= static_cast<int>(options.size())) return true;
                if (page.selected_bind_type == BindsActionType::Analog1D)
                    (void)ginput::add_axis_1d_bind(edited,
                        {options[static_cast<std::size_t>(choice)].code,
                         page.selected_bind_action});
                else (void)ginput::add_axis_2d_bind(edited,
                    {options[static_cast<std::size_t>(choice)].code,
                     page.selected_bind_action});
            } else if (page.selected_bind_type == BindsActionType::Button) {
                const auto bindings = ginput::button_binds_for_action(edited,
                    page.selected_bind_action);
                for (int index = 0; index < static_cast<int>(bindings.size()); ++index)
                    if (clear || index == mapping_index)
                        (void)ginput::remove_button_bind(edited,
                            bindings[static_cast<std::size_t>(index)]);
            } else if (page.selected_bind_type == BindsActionType::Analog1D) {
                const auto bindings = ginput::binds_for_axis_1d(edited,
                    page.selected_bind_action);
                for (int index = 0; index < static_cast<int>(bindings.size()); ++index)
                    if (clear || index == mapping_index)
                        (void)ginput::remove_axis_1d_bind(edited,
                            bindings[static_cast<std::size_t>(index)]);
            } else {
                const auto bindings = ginput::binds_for_axis_2d(edited,
                    page.selected_bind_action);
                for (int index = 0; index < static_cast<int>(bindings.size()); ++index)
                    if (clear || index == mapping_index)
                        (void)ginput::remove_axis_2d_bind(edited,
                            bindings[static_cast<std::size_t>(index)]);
            }
            page.toast = gubsy_replace_binds_profile(*page.backend, edited) ?
                "Mappings saved" : "Could not save mappings";
            if (action.starts_with("bind:choice:"))
                show_menu_screen(page, MenuScreen::BindDetail);
            page.dirty = true; return true;
        }
        const std::string encoded{action.substr(5)};
        const std::size_t separator = encoded.find(':');
        if (separator != std::string::npos) {
            page.selected_bind_type = static_cast<BindsActionType>(
                std::stoi(encoded.substr(0, separator)));
            page.selected_bind_action = std::stoi(encoded.substr(separator + 1));
            show_menu_screen(page, MenuScreen::BindDetail);
        }
        return true;
    }
    return false;
}
