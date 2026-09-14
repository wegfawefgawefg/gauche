#include "front_page.hpp"
#include "pages.hpp"
#include "profiles.hpp"
#include "text_edit.hpp"
#include "control_reference.hpp"
#include "audio.hpp"
#include "../graphics.hpp"
#include "../input.hpp"

#include <SDL3_image/SDL_image.h>
#include <gubsy/input/binds_profile.hpp>
#include "src/engine_state.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <string_view>

namespace {

struct TextureAsset { const char* id; const char* path; };
constexpr std::array<TextureAsset, 24> textures{{
    {"ui-action-green", "ui/menu/theme/action-green.png"},
    {"ui-action-green-dark", "ui/menu/theme/action-green-dark.png"},
    {"ui-bar-dark", "ui/menu/theme/bar-dark.png"},
    {"ui-button-light", "ui/menu/theme/button-light.png"},
    {"ui-group-inner", "ui/menu/theme/group-inner.png"},
    {"ui-parchment-ornate", "ui/menu/theme/parchment-ornate.png"},
    {"ui-slider-track", "ui/menu/theme/controls/slider-track.png"},
    {"ui-slider-fill", "ui/menu/theme/controls/slider-fill.png"},
    {"ui-slider-thumb", "ui/menu/theme/controls/slider-knob.png"},
    {"ui-toggle-off", "ui/menu/theme/controls/toggle-off.png"},
    {"ui-toggle-on", "ui/menu/theme/controls/toggle-on.png"},
    {"ui-scrollbar-track", "ui/menu/theme/controls/scrollbar-track.png"},
    {"ui-scrollbar-thumb", "ui/menu/theme/controls/scrollbar-thumb.png"},
    {"ui-stone-large-panel", "ui/menu/themes/splonks-stone/large-main-panel.png"},
    {"ui-stone-select-down", "ui/menu/themes/splonks-stone/select-down.png"},
    {"ui-stone-select-down-active", "ui/menu/themes/splonks-stone/select-down-active.png"},
    {"ui-stone-toggle-off", "ui/menu/themes/splonks-stone/toggle-off.png"},
    {"ui-stone-toggle-on", "ui/menu/themes/splonks-stone/toggle-on.png"},
    {"ui-stone-frame", "ui/menu/themes/splonks-stone/plain-stone.png"},
    {"ui-stone-normal-row", "ui/menu/themes/splonks-stone/normal-row.png"},
    {"ui-stone-selected-row", "ui/menu/themes/splonks-stone/selected-row.png"},
    {"ui-stone-outer-screen", "ui/menu/themes/splonks-stone/outer-screen-frame.png"},
    {"ui-stone-modal", "ui/menu/themes/splonks-stone/modal-frame.png"},
    {"ui-stone-toast", "ui/menu/themes/splonks-stone/toast-frame.png"},
}};

void nav(gview::InputFrame& frame, bool down, gview::NavAction action) {
    if (down) frame.navigation.push_back(action);
}

InputSettingsProfile* selected_tuning(FrontPage& page) {
    if (page.backend == nullptr) return nullptr;
    EngineState& engine = gubsy_runtime_engine(*page.backend);
    const auto found = std::find_if(engine.input_settings_profiles.begin(),
        engine.input_settings_profiles.end(), [&](const InputSettingsProfile& tuning) {
            return tuning.id == page.selected_profile;
        });
    return found == engine.input_settings_profiles.end() ? nullptr : &*found;
}

bool capture_button(FrontPage& page, const SDL_Event& event) {
    if (!page.capturing_bind || page.backend == nullptr) return false;
    if (profile_read_only(page)) { page.capturing_bind = false; return true; }
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
        page.capturing_bind = false;
        page.toast = "Binding capture cancelled";
        page.dirty = true;
        return true;
    }
    ginput::EncodedControl encoded = 0;
    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
        encoded = ginput::encode_button({ginput::DeviceKind::Keyboard,
            ginput::any_device_id, static_cast<int>(event.key.scancode)});
    else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN)
        encoded = ginput::encode_button({ginput::DeviceKind::Gamepad,
            ginput::any_device_id, static_cast<int>(event.gbutton.button)});
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        encoded = ginput::encode_button({ginput::DeviceKind::Mouse,
            ginput::any_device_id, static_cast<int>(SDL_BUTTON_MASK(event.button.button))});
    else return false;
    const BindsProfile* source = gubsy_find_binds_profile(*page.backend, page.selected_profile);
    if (source != nullptr) {
        BindsProfile edited = *source;
        (void)ginput::add_button_bind(edited,
            {encoded, page.selected_bind_action});
        page.toast = gubsy_replace_binds_profile(*page.backend, edited) ?
            "Binding saved" : "Could not save binding";
    }
    page.capturing_bind = false;
    page.dirty = true;
    return true;
}

std::string projection(const FrontPage& page, int death_policy) {
    std::string result = std::to_string(static_cast<int>(page.screen)) + ":" +
        std::to_string(death_policy) + ":" + page.toast + ":" +
        std::to_string(page.selected_profile) + ":" +
        std::to_string(static_cast<int>(page.selected_bind_type)) + ":" +
        std::to_string(page.selected_bind_action) + ":" +
        std::to_string(page.capturing_bind) + ":" +
        std::to_string(page.master_volume) + ":" +
        std::to_string(page.music_volume) + ":" +
        std::to_string(page.sfx_volume) + ":" +
        std::to_string(page.fullscreen) + ":" + std::to_string(page.vsync);
    result += ":" + std::to_string(page.window_mode) + ":" +
        std::to_string(page.render_resolution) + ":" +
        std::to_string(page.window_resolution) + ":" +
        std::to_string(page.frame_cap) + ":" + std::to_string(page.show_fps);
    if (page.backend != nullptr) {
        const GubsyLobbyState& lobby = gubsy_get_lobby_state(*page.backend);
        result += ":" + std::to_string(lobby.online) + ":" +
            std::to_string(lobby.is_host) + ":" + lobby.status_message + ":" +
            lobby.last_error + ":" + std::to_string(lobby.game_members.size()) + ":" +
            std::to_string(gubsy_get_binds_profiles(*page.backend).size());
    }
    result += ":" + std::to_string(controller_input_active()) + ":" +
        std::to_string(active_gamepad_id());
    return result;
}

} // namespace

bool init_front_page(FrontPage& page, GubsyRuntime& backend, SDL_Renderer* renderer) {
    if (page.painter) return true;
    page.backend = &backend;
    const auto root = asset_root();
    page.painter = std::make_unique<gview::Sdl3Renderer>(
        renderer, (root / "fonts" / "PixelOperator.ttf").string());
    if (!page.painter->ready()) return false;
    for (const TextureAsset& entry : textures) {
        const std::string path = (root / entry.path).string();
        SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
        if (texture == nullptr) {
            std::fprintf(stderr, "Gauche menu texture failed: %s: %s\n",
                         path.c_str(), SDL_GetError());
            return false;
        }
        (void)SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
        page.painter->register_texture(entry.id, texture);
        page.textures.emplace(entry.id, texture);
    }
    page.painter->register_surface("control-diagram", [&page](SDL_Renderer* target,
        const gview::PaintCommand& command) { draw_control_diagram(target, command, page); });
    return true;
}

void shutdown_front_page(FrontPage& page) {
    if (page.text_input_active) {
        const GubsyFrame frame = gubsy_get_frame(*page.backend);
        if (frame.window) (void)SDL_StopTextInput(frame.window);
        page.text_input_active = false;
    }
    for (auto& [id, texture] : page.textures) {
        if (page.painter) page.painter->unregister_texture(id);
        SDL_DestroyTexture(texture);
    }
    page.textures.clear();
    page.painter.reset();
}

void show_menu_screen(FrontPage& page, MenuScreen screen) {
    page.screen = screen;
    page.toast.clear();
    page.dirty = true;
}

bool front_page_event(FrontPage& page, const SDL_Event& event, const GubsyFrame& frame) {
    if (capture_button(page, event)) return true;
    if (event.type == SDL_EVENT_TEXT_INPUT) page.input.text += event.text.text;
    if (event.type == SDL_EVENT_KEY_DOWN &&
        (event.key.key == SDLK_BACKSPACE || event.key.key == SDLK_DELETE))
        page.input.text.push_back('\b');
    if (frame.window == nullptr || frame.render_width <= 0 || frame.render_height <= 0)
        return false;
    float mouse_x = 0.0F;
    float mouse_y = 0.0F;
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        mouse_x = event.motion.x; mouse_y = event.motion.y;
        page.input.pointer.moved = true;
    } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
               event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        mouse_x = event.button.x; mouse_y = event.button.y;
        if (event.button.button == SDL_BUTTON_LEFT) {
            page.input.pointer.pressed |= event.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
            page.input.pointer.released |= event.type == SDL_EVENT_MOUSE_BUTTON_UP;
        }
    } else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
        page.input.pointer.scroll_y += event.wheel.y;
        return false;
    } else return false;
    int window_width = 0;
    int window_height = 0;
    SDL_GetWindowSize(frame.window, &window_width, &window_height);
    const float scale = std::min(static_cast<float>(window_width) /
                                     static_cast<float>(frame.render_width),
                                 static_cast<float>(window_height) /
                                     static_cast<float>(frame.render_height));
    if (scale <= 0.0F) return false;
    page.input.pointer.x = (mouse_x - (static_cast<float>(window_width) -
                           static_cast<float>(frame.render_width) * scale) * 0.5F) / scale;
    page.input.pointer.y = (mouse_y - (static_cast<float>(window_height) -
                           static_cast<float>(frame.render_height) * scale) * 0.5F) / scale;
    return false;
}

std::string update_front_page(FrontPage& page, const MenuInputState& input,
                              int width, int height, int death_policy) {
    if (!page.painter) return {};
    if (!pointer_device_active()) page.input.pointer = {-1000, -1000, true, false, false, 0};
    const std::string view_key = projection(page, death_policy);
    if (!page.compiled || page.dirty || width != page.width || height != page.height ||
        view_key != page.projection) {
        gview::CompileResult result = gview::compile_view(
            build_menu_page(page, width, height, death_policy));
        if (!result.ok) {
            std::fprintf(stderr, "Gauche GView menu compile failed: %s\n",
                         result.diagnostics.empty() ? "unknown" :
                         result.diagnostics.front().message.c_str());
            return {};
        }
        if (page.compiled) page.runtime.reconcile(std::move(result.view));
        else page.runtime.reset(std::move(result.view));
        page.width = width;
        page.height = height;
        page.projection = view_key;
        page.compiled = true;
        page.dirty = false;
    }
    if (!page.capturing_bind) {
        nav(page.input, input.up, gview::NavAction::Up);
        nav(page.input, input.down, gview::NavAction::Down);
        nav(page.input, input.left, gview::NavAction::Left);
        nav(page.input, input.right, gview::NavAction::Right);
        nav(page.input, input.select, gview::NavAction::Confirm);
    }
    gview::Host host;
    host.read = [&page](std::string_view key) -> gview::Value {
        if (key == "join-host") return page.join_host;
        if (key == "join-port") return page.join_port;
        if (key == "host-port") return page.host_port;
        if (key == "profile-name") return page.profile_name;
        if (const InputSettingsProfile* tuning = selected_tuning(page)) {
            if (key == "input:controller-sensitivity")
                return static_cast<double>(tuning->controller_sensitivity);
            if (key == "input:stick-deadzone") return static_cast<double>(tuning->stick_deadzone);
            if (key == "input:trigger-threshold")
                return static_cast<double>(tuning->trigger_threshold);
            if (key == "input:controller-invert-x") return tuning->controller_invert_x;
            if (key == "input:controller-invert-y") return tuning->controller_invert_y;
        }
        return {};
    };
    host.write = [&page](std::string_view key, const gview::Value& value) {
        const std::string* text = std::get_if<std::string>(&value);
        if (text != nullptr) {
            if (key == "join-host") page.join_host = *text;
            if (key == "join-port") page.join_port = *text;
            if (key == "host-port") page.host_port = *text;
            if (key == "profile-name" && !profile_read_only(page)) {
                page.profile_name = *text;
                (void)save_profile_name(page);
            }
            return;
        }
        InputSettingsProfile* tuning = selected_tuning(page);
        if (tuning == nullptr || profile_read_only(page)) return;
        if (const double* number = std::get_if<double>(&value)) {
            const float level = static_cast<float>(*number);
            if (key == "input:controller-sensitivity") tuning->controller_sensitivity = level;
            if (key == "input:stick-deadzone") tuning->stick_deadzone = level;
            if (key == "input:trigger-threshold") tuning->trigger_threshold = level;
        }
        if (const bool* enabled = std::get_if<bool>(&value)) {
            if (key == "input:controller-invert-x") tuning->controller_invert_x = *enabled;
            if (key == "input:controller-invert-y") tuning->controller_invert_y = *enabled;
        }
        (void)save_input_settings_profile(*tuning);
        menu_feedback(page, gview::FeedbackEvent::Toggle);
    };
    host.feedback = [&page](gview::FeedbackEvent event, gview::NodeIndex node) {
        if (event == gview::FeedbackEvent::Activate &&
            page.runtime.view().nodes[node].source.action == "back") return;
        menu_feedback(page, event);
    };
    host.action = [&page](std::string_view action, gview::NodeIndex) {
        page.action = action;
    };
    glayout::ResolveInput resolution{};
    resolution.viewport = {0.0F, 0.0F, static_cast<float>(width),
                           static_cast<float>(height)};
    finish_text_edit(page, input, resolution, host);
    page.runtime.frame(resolution, page.input, host);
    update_control_preview(page);
    std::string hovered;
    for (gview::NodeIndex i = 0; i < page.runtime.state().size(); ++i)
        if (page.runtime.state()[i].hovered)
            hovered = page.runtime.view().nodes[i].source.layout_id;
    if (!hovered.empty() && hovered != page.hovered_control && page.input.pointer.moved)
        menu_feedback(page, gview::FeedbackEvent::Move);
    page.hovered_control = std::move(hovered);
    const float pointer_x = page.input.pointer.x;
    const float pointer_y = page.input.pointer.y;
    page.input = {};
    page.input.pointer.x = pointer_x;
    page.input.pointer.y = pointer_y;
    const auto& state = page.runtime.state();
    const bool editing = std::any_of(state.begin(), state.end(),
        [](const gview::NodeState& node) { return node.editing; });
    if (editing != page.text_input_active) {
        const GubsyFrame frame = gubsy_get_frame(*page.backend);
        if (frame.window != nullptr) {
            if (editing) (void)SDL_StartTextInput(frame.window);
            else (void)SDL_StopTextInput(frame.window);
        }
        page.text_input_active = editing;
    }
    std::string action = std::move(page.action);
    page.action.clear();
    return action;
}

void render_front_page(FrontPage& page) {
    if (page.compiled && page.painter) page.painter->render(page.runtime.paint());
}
