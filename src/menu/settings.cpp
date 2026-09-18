#include "settings.hpp"
#include "../menu_shell.hpp"
#include "../app/options.hpp"
#include "../input/icon_set.hpp"
#include "../audio.hpp"
#include "src/engine_state.hpp"
#include "src/graphics.hpp"
#include <gubsy/settings/top_level_settings.hpp>
#include <gubsy/settings/audio.hpp>
#include <algorithm>
#include <cmath>
#include <charconv>
#ifdef __EMSCRIPTEN__
#include "../browser/display.hpp"
#include <emscripten.h>
#endif

std::optional<gview::Value> read_menu_setting(const FrontPage& p, std::string_view key) {
    for (auto i = p.setting_edits.rbegin(); i != p.setting_edits.rend(); ++i)
        if (i->first == key) return i->second;
    if (key == "setting:master") return std::round(p.master_volume * 100.0);
    if (key == "setting:music") return std::round(p.music_volume * 100.0);
    if (key == "setting:sfx") return std::round(p.sfx_volume * 100.0);
    if (key == "setting:icons") return std::string(controller_icon_name());
    if (key == "setting:vsync") return p.vsync;
    if (key == "setting:show-fps") return p.show_fps;
    if (key == "setting:auto-reports") return p.auto_reports;
    if (key == "setting:render-scale") return std::to_string(p.browser_render_percent) + "%";
    if (key == "setting:window-mode") {
        constexpr const char* names[]{"Windowed", "Borderless", "Fullscreen"};
        return std::string(names[std::clamp(p.window_mode, 0, 2)]);
    }
    if (!p.backend) return {};
    const auto& engine = gubsy_runtime_engine(*p.backend);
    if (key == "setting:frame-limit") {
        const auto limit = get_top_level_setting_string(engine.top_level_game_settings, "gubsy.video.frame_cap", "0");
#ifdef __EMSCRIPTEN__
        if (limit == "0") return std::string("Display refresh rate");
#else
        if (limit == "0") return std::string("Unlimited");
#endif
        return limit + " FPS";
    }
    if (key == "setting:render-resolution" && render_resolution_matches_window(engine))
        return std::string("Match window");
    if (key == "setting:render-resolution" || key == "setting:window-resolution") {
        const auto size = key == "setting:render-resolution" ? get_render_dimensions(engine) : get_window_dimensions(engine);
        return std::to_string(size.x) + "x" + std::to_string(size.y);
    }
    return {};
}

void apply_menu_setting(MenuShell& menu, std::string_view key, const gview::Value& value) {
    auto& p = menu.front;
    auto& engine = gubsy_runtime_engine(*menu.runtime);
    auto& settings = engine.top_level_game_settings;
    const auto* text = std::get_if<std::string>(&value);
    const auto* enabled = std::get_if<bool>(&value);
    const auto* number = std::get_if<double>(&value);
    if (key == "setting:master" || key == "setting:music" || key == "setting:sfx") {
        if (!number || !std::isfinite(*number)) return;
        const float level = static_cast<float>(std::clamp(std::round(*number), 0.0, 100.0)) / 100.F;
        float& field = key == "setting:master" ? p.master_volume : key == "setting:music" ? p.music_volume : p.sfx_volume;
        if (field == level) return;
        field = level;
        engine.audio_settings.vol_master = p.master_volume;
        engine.audio_settings.vol_music = p.music_volume;
        engine.audio_settings.vol_sfx = p.sfx_volume;
        set_top_level_setting_float(settings, "gubsy.audio.master_volume", p.master_volume);
        set_top_level_setting_float(settings, "gubsy.audio.music_volume", p.music_volume);
        set_top_level_setting_float(settings, "gubsy.audio.sfx_volume", p.sfx_volume);
        const auto path = user_data_root() / "gubsy/settings_profiles/audio.lisp";
        if (!save_audio_settings(engine, path.string()))
            p.toast = "Could not save audio settings";
        else if (p.audio) sync_audio_settings(*p.audio, path);
    } else if (key == "setting:icons" && text) {
        constexpr const char* names[]{"Auto", "Xbox", "PlayStation", "Nintendo"};
        for (int i=0; i<4; ++i) if (*text == names[i]) {
            if (!set_controller_icons(static_cast<ControllerIcons>(i))) p.toast = "Could not save controller icons";
            p.dirty = true; return;
        }
        return;
    } else if (key == "setting:show-fps" && enabled) {
        p.show_fps = *enabled;
        set_top_level_setting_int(settings, "gubsy.video.show_fps", *enabled ? 1 : 0);
    } else if (key == "setting:frame-limit" && text) {
        int fps = -1;
        if (*text == "Unlimited" || *text == "Display refresh rate") fps = 0;
        for (int n : {30,60,90,120,144,165,240}) if (*text == std::to_string(n) + " FPS") fps = n;
        if (fps < 0) return;
        set_top_level_setting_string(settings, "gubsy.video.frame_cap", std::to_string(fps));
#ifdef __EMSCRIPTEN__
    } else if (key == "setting:auto-reports" && enabled) {
        EM_ASM({Module.setAutoReports(!!$0);}, *enabled ? 1 : 0);
        p.auto_reports = *enabled;
    } else if (key == "setting:render-scale" && text) {
        const int percent = *text == "100%" ? 100 : *text == "75%" ? 75 : *text == "50%" ? 50 : 0;
        if (!percent) return;
        const int previous = p.browser_render_percent;
        p.browser_render_percent = percent;
        if (!sync_browser_render_resolution(menu)) { p.browser_render_percent = previous; return; }
        set_top_level_setting_int(settings, "teeming.video.browser_render_percent", percent);
#else
    } else if (key == "setting:vsync" && enabled) {
        if (!tr::set_vsync(gubsy_get_frame(*menu.runtime).renderer, *enabled ? 1 : 0)) {
            p.toast = "Could not change V-sync"; p.dirty = true; return;
        }
        p.vsync = *enabled;
        set_top_level_setting_int(settings, "gubsy.video.vsync", *enabled ? 1 : 0);
    } else if (key == "setting:window-mode" && text) {
        const int mode = *text == "Windowed" ? 0 : *text == "Borderless" ? 1 : *text == "Fullscreen" ? 2 : -1;
        if (mode < 0) return;
        if (!set_window_display_mode(engine, static_cast<WindowDisplayMode>(mode))) {
            p.toast = "Could not change window mode"; p.dirty = true; return;
        }
        p.window_mode = mode; p.fullscreen = mode == 2;
        constexpr const char* names[]{"windowed", "borderless", "fullscreen"};
        set_top_level_setting_string(settings, "gubsy.video.window_mode", names[mode]);
    } else if ((key == "setting:render-resolution" || key == "setting:window-resolution") && text) {
        const bool render = key == "setting:render-resolution";
        if (render && *text == "Match window") {
            set_top_level_setting_int(settings, "gubsy.video.match_render_to_window", 1);
            sync_matched_render_resolution(engine);
        } else {
            const auto split = text->find('x');
            if (split == std::string::npos) return;
            int w=0,h=0;
            const auto a=std::from_chars(text->data(),text->data()+split,w);
            const auto b=std::from_chars(text->data()+split+1,text->data()+text->size(),h);
            if (a.ec!=std::errc{} || a.ptr!=text->data()+split || b.ec!=std::errc{} || b.ptr!=text->data()+text->size() || w<320 || h<180 || w>7680 || h>4320) return;
            if (!(render ? set_render_resolution(engine,w,h) : set_window_dimensions(engine,w,h))) {
                p.toast="Could not change resolution"; p.dirty=true; return;
            }
            if (render) set_top_level_setting_int(settings,"gubsy.video.match_render_to_window",0);
            set_top_level_setting_string(settings,render ? "gubsy.video.render_resolution" : "gubsy.video.window_resolution",*text);
        }
#endif
    } else return;
    if (!save_top_level_game_settings(settings)) { p.toast="Could not save settings"; p.dirty=true; }
    // Values are read by GView without rebuilding the view during slider dragging.
}

void apply_pending_settings(MenuShell& menu) {
    auto edits = std::move(menu.front.setting_edits);
    menu.front.setting_edits.clear();
    for (const auto& [key,value] : edits) apply_menu_setting(menu,key,value);
}
