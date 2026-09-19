#include "../input/icon_set.hpp"
#include "control_pages.hpp"
#include "page_chrome.hpp"
#include "profiles.hpp"
#include "control_reference.hpp"

namespace teeming_menu {
namespace {
std::string button_label(int code) {
    ginput::DeviceButton decoded;
    if (!ginput::decode_button(code, decoded))
        return binds_input_label(BindsActionType::Button, code);
    if (decoded.kind == ginput::DeviceKind::Keyboard) {
        const char* name = SDL_GetScancodeName(static_cast<SDL_Scancode>(decoded.code));
        return "Keyboard " + std::string{name == nullptr ? "Unknown" : name};
    }
    if (decoded.kind == ginput::DeviceKind::Gamepad) {
        const char* name = SDL_GetGamepadStringForButton(
            static_cast<SDL_GamepadButton>(decoded.code));
        return "Gamepad " + std::string{name == nullptr ? "Unknown" : name};
    }
    if (decoded.kind == ginput::DeviceKind::Mouse)
        return "Mouse button " + std::to_string(decoded.code);
    return "Device button " + std::to_string(decoded.code);
}

} // namespace

// PROFILES: Selection and editing are separate actions on each row.
void controls_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Control Profiles", 1140, 640);
    ui.layout_container("card", "profile-toolbar", glayout::ContainerKind::Row,
        {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Pixels, 42}, 12);
    ui.label("profile-toolbar", "control-help", "Select controls for Player " +
        std::to_string(page.selected_player + 1), 42, 17);
    ui.button("profile-toolbar", "profile-new", "+ Create Profile", "profile:new", "menu", 42);
    ui.layout("profile-new").size.width = {glayout::LengthKind::Pixels, 178 * ui.scale()};
    ui.layout_container("card", "profile-body", glayout::ContainerKind::Row,
        {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Fill, 1}, 24);
    ui.layout_container("profile-body", "profiles", glayout::ContainerKind::Column,
        {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Fill, 1}, 8);
    ui.scrolling("profiles");
    std::string first = "profile-new";
    for (const BindsProfile& profile : gubsy_get_binds_profiles(*page.backend)) {
        const std::string id = std::to_string(profile.id);
        const bool active = active_profile_id(page) == profile.id;
        const bool defaults = default_profile(profile);
        const std::string name = defaults ? "Defaults" : profile.name;
        ui.layout_container("profiles", "profile-row-" + id, glayout::ContainerKind::Row,
            {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Pixels, 48}, 8);
        ui.button("profile-row-" + id, "profile-" + id,
            name + (active ? "  ·  ACTIVE" : ""), "profile:use:" + id, "menu", 48);
        ui.spec("profile-" + id).selected = active;
        ui.button("profile-row-" + id, "edit-" + id, defaults ? "View" : "Edit",
            "profile:" + id, "menu", 48);
        ui.layout("edit-" + id).size.width = {glayout::LengthKind::Pixels, 104 * ui.scale()};
        if (active) first = "profile-" + id;
    }
    control_reference(ui, "profile-body", page,
        page.preview_profile < 0 ? active_profile_id(page) : page.preview_profile);
    ui.label("card", "profile-help", "New profiles start with Teeming's default controls.", 28, 15);
    footer(ui, first);
}

void profile_editor_page(ViewBuilder& ui, const FrontPage& page) {
    const bool read_only = profile_read_only(page);
    frame(ui, read_only ? "Default Controls" : "Edit Controls", 1100, 610);
    ui.layout_container("card", "editor-body", glayout::ContainerKind::Row,
        {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Fill, 1}, 24);
    ui.layout_container("editor-body", "editor-fields", glayout::ContainerKind::Column,
        {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Fill, 1}, 8);
    if (read_only)
        ui.label("editor-fields", "default-note", "Defaults are read-only. Duplicate them to make changes.", 48, 17);
    else
        ui.text_input("editor-fields", "profile-name", "Profile name", "profile-name", "menu", 48);
    ui.button("editor-fields", "profile-use", active_profile_id(page) == page.selected_profile ?
        "Active for Player " + std::to_string(page.selected_player + 1) :
        "Use for Player " + std::to_string(page.selected_player + 1),
        "profile:use:" + std::to_string(page.selected_profile), "menu", 46);
    ui.spec("profile-use").selected = active_profile_id(page) == page.selected_profile;
    ui.button("editor-fields", "profile-bindings", "Button and Analog Mappings", "profile:bindings", "menu", 46);
    ui.button("editor-fields", "profile-input", "Controller Options", "input-options", "menu", 46);
    ui.button("editor-fields", "profile-copy", "Duplicate Profile", "profile:copy", "menu", 46);
    if (!read_only)
        ui.label("editor-fields", "profile-note", "Changes save automatically when you finish editing.", 32, 15);
    control_reference(ui, "editor-body", page, page.selected_profile);
    footer(ui, read_only ? "profile-bindings" : "profile-name");
}

void bindings_page(ViewBuilder& ui, const FrontPage& page) {
    const BindsProfile* profile = gubsy_find_binds_profile(*page.backend,
                                                            page.selected_profile);
    frame(ui, profile ? profile->name + " · Bindings" : "Bindings", 900.0F, 620.0F);
    ui.layout_container("card", "binding-list", glayout::ContainerKind::Column,
                        {glayout::LengthKind::Fill, 1.0F},
                        {glayout::LengthKind::Fill, 1.0F}, 4.0F);
    ui.scrolling("binding-list");
    const BindsSchema& schema = get_binds_schema();
    for (const ginput::SchemaEntry& entry : schema.actions()) {
        std::string mapping;
        if (profile != nullptr)
            for (const ginput::ButtonBind& bind :
                 ginput::button_binds_for_action(*profile, entry.id)) {
                if (!mapping.empty()) mapping += ", ";
                mapping += button_label(bind.device_button);
            }
        if (mapping.empty()) mapping = "Unbound";
        ui.button("binding-list", "bind-" + std::to_string(entry.id),
                  entry.label + "  ·  " + mapping,
                  "bind:0:" + std::to_string(entry.id),
                  "menu", 39.0F);
    }
    for (const ginput::SchemaEntry& entry : schema.axes_1d())
        ui.button("binding-list", "axis1-" + std::to_string(entry.id),
                  entry.label + "  ·  Analog", "bind:1:" + std::to_string(entry.id),
                  "menu", 39.0F);
    for (const ginput::SchemaEntry& entry : schema.axes_2d())
        ui.button("binding-list", "axis2-" + std::to_string(entry.id),
                  entry.label + "  ·  Analog", "bind:2:" + std::to_string(entry.id),
                  "menu", 39.0F);
    ui.label("card", "binding-help", page.capturing_bind ?
             "Press a key, mouse button, or gamepad button…" :
             "Select an action to view, add, or remove mappings.", 34.0F, 15.0F);
    button(ui, "input-options", "Stick and trigger options", "input-options", 42.0F);
    footer(ui, schema.actions().empty() ? "back" :
               "bind-" + std::to_string(schema.actions().front().id));
}

void input_options_page(ViewBuilder& ui, const FrontPage& page) {
    const bool read_only = profile_read_only(page);
    frame(ui, "Controller Options", 760, 500);
    ui.layout_container("card", "input-list", glayout::ContainerKind::Column,
                        {glayout::LengthKind::Fill, 1.0F},
                        {glayout::LengthKind::Fill, 1.0F}, 5.0F);
    ui.scrolling("input-list");
    ui.select("input-list", "controller-icons", "Button icons", "setting:icons",
              {"Auto", "Xbox", "PlayStation", "Nintendo"}, "menu", 42);
    ui.slider("input-list", "controller-sensitivity", "Controller sensitivity",
              "input:controller-sensitivity", "menu", 0.4, 2.0, 0.05, 46.0F);
    ui.slider("input-list", "stick-deadzone", "Stick deadzone",
              "input:stick-deadzone", "menu", 0.0, 0.95, 0.01, 46.0F);
    ui.slider("input-list", "trigger-threshold", "Trigger threshold",
              "input:trigger-threshold", "menu", 0.01, 1.0, 0.01, 46.0F);
    ui.toggle("input-list", "controller-invert-x", "Invert controller X",
              "input:controller-invert-x", "menu", 42.0F);
    ui.toggle("input-list", "controller-invert-y", "Invert controller Y",
              "input:controller-invert-y", "menu", 42.0F);
    for (const char* id : {"controller-sensitivity", "stick-deadzone", "trigger-threshold",
                           "controller-invert-x", "controller-invert-y"})
        ui.spec(id).enabled = !read_only;
    ui.label("card", "input-note", read_only ? "Duplicate Defaults to adjust these settings." :
        "Deadzone ignores stick drift. Trigger threshold controls when use starts.", 38, 15);
    footer(ui, read_only ? "back" : "controller-sensitivity");
}

void binding_detail_page(ViewBuilder& ui, const FrontPage& page) {
    const BindsProfile* profile = gubsy_find_binds_profile(*page.backend,
                                                            page.selected_profile);
    const BindsSchema& schema = get_binds_schema();
    const ginput::SchemaEntry* entry = page.selected_bind_type == BindsActionType::Button ?
        schema.find_action(page.selected_bind_action) :
        (page.selected_bind_type == BindsActionType::Analog1D ?
         schema.find_axis_1d(page.selected_bind_action) :
         schema.find_axis_2d(page.selected_bind_action));
    frame(ui, entry ? entry->label : "Binding", 760.0F, 520.0F);
    const bool read_only = profile_read_only(page);
    ui.label("card", "binding-instruction", read_only ? "Default mappings · read-only" :
        "Existing mappings", 36, 17);
    const auto mapping_row = [&](std::string label, int index) {
        if (read_only) ui.label("card", "mapping-" + std::to_string(index), label, 40, 17);
        else button(ui, "mapping-" + std::to_string(index), label + "  ·  Remove",
            "bind:remove:" + std::to_string(index));
    };
    int index = 0;
    if (profile != nullptr && page.selected_bind_type == BindsActionType::Button)
        for (const ginput::ButtonBind& mapping :
             ginput::button_binds_for_action(*profile, page.selected_bind_action)) {
            mapping_row(button_label(mapping.device_button), index);
            ++index;
        }
    if (profile != nullptr && page.selected_bind_type == BindsActionType::Analog1D)
        for (const ginput::Axis1DBind& mapping :
             ginput::binds_for_axis_1d(*profile, page.selected_bind_action)) {
            mapping_row(binds_input_label(BindsActionType::Analog1D, mapping.device_axis), index);
            ++index;
        }
    if (profile != nullptr && page.selected_bind_type == BindsActionType::Analog2D)
        for (const ginput::Axis2DBind& mapping :
             ginput::binds_for_axis_2d(*profile, page.selected_bind_action)) {
            mapping_row(binds_input_label(BindsActionType::Analog2D, mapping.device_stick), index);
            ++index;
        }
    if (index == 0) ui.label("card", "no-mapping", "No mappings yet", 40.0F, 15.0F);
    if (!read_only) {
        button(ui, "binding-add", page.capturing_bind ? "Press a button…" :
            "+ Add Mapping", "bind:add");
        button(ui, "binding-clear", "Clear Action", "bind:clear");
    }
    footer(ui, read_only ? "back" : "binding-add");
}

void binding_choices_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Choose Analog Input", 720.0F, 510.0F);
    ui.layout_container("card", "choice-list", glayout::ContainerKind::Column,
                        {glayout::LengthKind::Fill, 1.0F},
                        {glayout::LengthKind::Fill, 1.0F}, 4.0F);
    ui.scrolling("choice-list");
    const std::vector<InputChoice>& choices = binds_input_choices(page.selected_bind_type);
    for (std::size_t index = 0; index < choices.size(); ++index)
        ui.button("choice-list", "choice-" + std::to_string(index), choices[index].label,
                  "bind:choice:" + std::to_string(index), "menu", 42.0F);
    footer(ui, choices.empty() ? "back" : "choice-0");
}

} // namespace teeming_menu
