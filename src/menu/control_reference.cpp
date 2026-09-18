#include "control_reference.hpp"
#include "profiles.hpp"
#include "../input.hpp"
#include "../ui/prompts.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>

// REFERENCE: A compact live legend beside the chosen profile and pause actions.
void control_reference(gauche_menu::ViewBuilder& ui, std::string_view parent,
                       const FrontPage& page, int id) {
    const auto* profile = gubsy_find_binds_profile(*page.backend, id);
    const bool pad = controller_input_active();
    ui.layout_container(parent, "control-reference", glayout::ContainerKind::Column,
        {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Fill, 1}, 2);
    ui.scrolling("control-reference");
    const std::string name = profile == nullptr || default_profile(*profile) ? "Defaults" : profile->name;
    ui.label("control-reference", "reference-heading", name + " · " +
        (pad ? "Gamepad" : "Keyboard / Mouse"), 28, 19);
    ui.surface("control-reference", "control-diagram", "control-diagram", 90);
    const auto row = [&](const char* key, std::string label, std::string binding) {
        ui.label("control-reference", std::string{"reference-"} + key,
            std::move(binding) + "   ·   " + label, 24, 18);
    };
    const auto bind = [&](Action action) { return action_prompt(action, id).label; };
    row("move", "Move", pad ? stick_prompt(0, id).label + " / " + bind(Action::MoveUp) :
        bind(Action::MoveUp) + " " + bind(Action::MoveLeft) + " " +
        bind(Action::MoveDown) + " " + bind(Action::MoveRight));
    row("aim", "Aim", pad ? stick_prompt(1, id).label : "Mouse");
    row("use", "Use / draw bow", bind(Action::Use));
    row("pickup", "Pick up / interact / drop / swap", bind(Action::Pickup));
    row("reload", "Reload / secondary", bind(Action::Reload));
    if (!pad) row("interact", "Interact", bind(Action::Interact));
    row("confirm", "Menu confirm", pad ? pad_button_prompt(SDL_GAMEPAD_BUTTON_SOUTH).label : "Enter");
    row("cancel", "Menu cancel", pad ? pad_button_prompt(SDL_GAMEPAD_BUTTON_EAST).label : "Esc");
    row("inventory", "Inventory", bind(Action::Inventory));
    row("compare", "Compare", bind(Action::Compare));
    row("details", "Item details", bind(Action::CompactDetails));
    row("slots", "Select item", pad ? bind(Action::PreviousSlot) + " / " +
        bind(Action::NextSlot) : bind(Action::Slot1) + " – " + bind(Action::Slot6));
}

// DIAGRAM: Physical layout uses SDL's face labels; the legend explains bindings.
void draw_control_diagram(tr::Renderer* renderer, const gview::PaintCommand& command,
                          const FrontPage& page) {
    float sx = 1, sy = 1;
    tr::get_scale(renderer, &sx, &sy);
    const float scale = std::min(command.rect.w / 210, command.rect.h / 76);
    if (scale <= 0) return;
    const bool clipped = tr::clip_enabled(renderer);
    SDL_Rect clip{};
    tr::get_clip(renderer, &clip);
    tr::set_scale(renderer, sx * scale, sy * scale);
    if (clipped) {
        const SDL_Rect scaled{static_cast<int>(std::floor(static_cast<float>(clip.x) / scale)),
            static_cast<int>(std::floor(static_cast<float>(clip.y) / scale)),
            static_cast<int>(std::ceil(static_cast<float>(clip.w) / scale)),
            static_cast<int>(std::ceil(static_cast<float>(clip.h) / scale))};
        tr::set_clip(renderer, &scaled);
    }
    const float x = command.rect.x / scale + (command.rect.w / scale - 210) * .5F;
    const float y = command.rect.y / scale;
    if (controller_input_active()) {
        tr::set_color_bytes(renderer, 102, 111, 102, 255);
        SDL_FPoint outline[]{{x+48,y+16},{x+163,y+16},{x+177,y+28},{x+187,y+66},
            {x+174,y+71},{x+149,y+51},{x+63,y+51},{x+38,y+71},{x+25,y+66},
            {x+35,y+28},{x+48,y+16}};
        SDL_Vertex vertices[12]{};
        vertices[0] = {{x+106, y+34}, {.16F, .19F, .16F, 1}, {0, 0}};
        for (int i=0; i<11; ++i) vertices[i+1] = {outline[i], {.16F, .19F, .16F, 1}, {0, 0}};
        int triangles[30]{};
        for (int i=0; i<10; ++i) {
            triangles[i*3] = 0; triangles[i*3+1] = i+1; triangles[i*3+2] = i+2;
        }
        tr::geometry(renderer, nullptr, vertices, 12, triangles, 30);
        draw_prompt(renderer, x+45, y+1, pad_button_prompt(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER));
        draw_prompt(renderer, x+150, y+1, pad_button_prompt(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER));
        constexpr SDL_GamepadButton buttons[]{SDL_GAMEPAD_BUTTON_NORTH,
            SDL_GAMEPAD_BUTTON_WEST, SDL_GAMEPAD_BUTTON_EAST, SDL_GAMEPAD_BUTTON_SOUTH};
        constexpr SDL_FPoint positions[]{{157,20},{141,32},{173,32},{157,44}};
        for (int i=0; i<4; ++i)
            draw_prompt(renderer, x+positions[i].x, y+positions[i].y, pad_button_prompt(buttons[i]));
        draw_prompt(renderer, x+43, y+30, {"+", true});
        draw_prompt(renderer, x+76, y+47, {"L", true});
        draw_prompt(renderer, x+115, y+47, {"R", true});
    } else {
        const int id = page.screen == MenuScreen::Controls ?
            (page.preview_profile < 0 ? active_profile_id(page) : page.preview_profile) :
            page.screen == MenuScreen::Pause ? active_profile_id(page) : page.selected_profile;
        constexpr Action actions[]{Action::MoveUp, Action::MoveLeft, Action::MoveDown, Action::MoveRight};
        constexpr SDL_FPoint positions[]{{70,14},{38,32},{70,32},{102,32}};
        for (int i=0; i<4; ++i)
            draw_prompt(renderer, x+positions[i].x, y+positions[i].y, action_prompt(actions[i], id));
        draw_prompt(renderer, x+48, y+51, action_prompt(Action::Use, id));
        tr::set_color_bytes(renderer, 155, 163, 143, 255);
        const SDL_FRect mouse{x+155,y+16,24,37};
        tr::fill_rect(renderer, &mouse);
        tr::set_color_bytes(renderer, 42, 48, 42, 255);
        const SDL_FRect split{x+166,y+16,2,16}, wheel{x+164,y+24,6,10};
        tr::fill_rect(renderer, &split); tr::fill_rect(renderer, &wheel);
    }
    tr::set_scale(renderer, sx, sy);
    tr::set_clip(renderer, clipped ? &clip : nullptr);
}

void update_control_preview(FrontPage& page) {
    if (page.screen != MenuScreen::Controls) return;
    auto focus = page.runtime.focus();
    // Hovering a row previews it without changing the active profile.
    for (gview::NodeIndex i = 0; i < page.runtime.state().size(); ++i)
        if (page.runtime.state()[i].hovered) { focus = i; break; }
    if (focus == gview::invalid_node) return;
    std::string_view name = page.runtime.view().nodes[focus].source.layout_id;
    if (name.starts_with("profile-")) name.remove_prefix(8);
    else if (name.starts_with("edit-")) name.remove_prefix(5);
    else return;
    int id = -1;
    const auto [end, error] = std::from_chars(name.data(), name.data()+name.size(), id);
    if (error != std::errc{} || end != name.data()+name.size() || id == page.preview_profile) return;
    page.preview_profile = id;
    page.dirty = true;
}
