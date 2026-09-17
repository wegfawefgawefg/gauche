#include "worldgen.hpp"
#include "worldgen_sidebar.hpp"
#include "playtest.hpp"
#include "panels.hpp"
#include "generation_build.hpp"
#include "../menu_shell.hpp"
#include <algorithm>
#include <cmath>

WorldGenViewer& worldgen_viewer() { static WorldGenViewer viewer; return viewer; }

void fit_worldgen(WorldGenViewer& v) {
    if (!v.original) return;
    focus_worldgen_bounds(v,{},Cell{v.original->stage.width,v.original->stage.height},8.0F);
}

void select_worldgen_checkpoint(WorldGenViewer& v,int checkpoint) {
    v.checkpoint=std::clamp(checkpoint,0,std::max(0,static_cast<int>(v.trace.checkpoints.size())-1));
    if (v.follow_step && !v.trace.checkpoints.empty()) {
        const auto& step=v.trace.checkpoints[static_cast<std::size_t>(v.checkpoint)];
        if (step.detail) {
            v.selected_component=step.component;
            if(step.component>=0 && static_cast<std::size_t>(step.component)<step.report.components.size())
                v.selected_feature=static_cast<int>(step.report.components[static_cast<std::size_t>(step.component)].feature);
        }
    }
    if(!v.trace.checkpoints.empty()) {
        const auto& report=v.trace.checkpoints[static_cast<std::size_t>(v.checkpoint)].report;
        if(v.selected_component>=static_cast<int>(report.components.size()))v.selected_component=-1;
        if(v.sidebar_children && v.selected_component<0) {
            for(std::size_t i=0;i<report.components.size();++i)
                if(static_cast<int>(report.components[i].feature)==v.selected_feature){v.selected_component=static_cast<int>(i);break;}
            if(v.selected_component<0)v.sidebar_children=false;
        }
    }
}
void recapture_worldgen(WorldGenViewer& v) {
    if (!v.original) return;
    v.seed=v.original->run.seed;v.floor=v.original->run.floor;
    v.inhabitants_seed=v.original->generation_report ? v.original->generation_report->inhabitants_seed : 0;
    v.keep_view_on_regen=true;v.regenerate_requested=true;
}
void reroll_worldgen_inhabitants(WorldGenViewer& v) {
    if(!v.original || v.original->run.layout!=FloorLayout::Generated)return;
    recapture_worldgen(v);
    if(++v.inhabitants_seed==0)v.inhabitants_seed=1;
}
std::string worldgen_recipe(const GenerationReport& report) {
    return "floor "+std::to_string(report.floor)+" | seed "+std::to_string(report.seed)+
        " | planner RNG "+std::to_string(report.initial_rng)+
        " | inhabitants seed "+std::to_string(report.inhabitants_seed)+
        " (0=planner stream; override after boss geometry) | revision "+
        (report.revision.empty() ? GAUCHE_GENERATOR_REVISION : report.revision);
}
void regenerate_worldgen(WorldGenViewer& v) {
    v.original=std::make_unique<Game>();
    v.population={};
    v.selected_component=-1;v.sidebar_children=false;
    Game& game=*v.original;
    game.rng=v.seed==0 ? 1 : v.seed;
    game.run.seed=game.rng;
    game.run.floor=v.floor;
    game.run.phase=RunPhase::Playing;
    player_state(game, 0).online=true;
    v.trace.options=v.capture_options;
    generate_world_floor(game,FloorLayout::Automatic,&v.population,&v.trace,v.inhabitants_seed);
    v.checkpoint=static_cast<int>(v.trace.checkpoints.size())-1;
    if (!v.keep_view_on_regen) fit_worldgen(v);
    v.keep_view_on_regen=false;
}

void process_worldgen_requests(MenuShell& menu) {
    auto& v=worldgen_viewer();
    if (menu.network->role!=NetRole::Solo || menu.rooms.active || menu.rooms.busy) {
        v.active=v.playing=v.open_requested=v.return_requested=v.play_requested=false;
        v.regenerate_requested=v.exit_requested=false;
        return;
    }
    if (v.exit_requested) {
        v.active=v.playing=v.exit_requested=false;
        show_title_menu(menu);
    }
    if (v.open_requested || v.return_requested) {
        if (!v.original) { v.seed=SDL_GetTicks()+1; regenerate_worldgen(v); }
        v.active=true; v.playing=false;
        playtest_tools().jump_requested=playtest_tools().equip_requested=false;
        menu.playing=false; menu.visible=menu.front_visible=false;
        v.open_requested=v.return_requested=false;
        ++playtest_tools().revision;
    }
    if (v.regenerate_requested) {
        regenerate_worldgen(v); v.regenerate_requested=false;
    }
    if (v.play_requested && v.active && v.original) {
        *menu.solo_game=*v.original;
        v.active=false; v.playing=true; v.play_requested=false;
        menu.playing=true; menu.visible=menu.front_visible=false;
        debug_panels().visible=false;
        ++playtest_tools().revision;
    }
    if (v.playing && !menu.playing) v.playing=false;
}

bool worldgen_event(const SDL_Event& event) {
    auto& v=worldgen_viewer();
    if (!v.active) {
        if (v.playing && ((event.type==SDL_EVENT_KEY_DOWN && event.key.key==SDLK_F6) ||
            (event.type==SDL_EVENT_GAMEPAD_BUTTON_DOWN && event.gbutton.button==SDL_GAMEPAD_BUTTON_BACK))) {
            v.return_requested=true; return true;
        }
        return false;
    }
    if(event.type==SDL_EVENT_KEY_DOWN && event.key.repeat &&
        (event.key.key==SDLK_ESCAPE || event.key.key==SDLK_RETURN || event.key.key==SDLK_TAB)) return true;
    if (worldgen_sidebar_event(event,v)) return true;
    if (event.type==SDL_EVENT_MOUSE_WHEEL) {
        v.zoom=std::clamp(v.zoom*std::pow(1.15F,event.wheel.y),.08F,8.0F); return true;
    }
    const bool key=event.type==SDL_EVENT_KEY_DOWN;
    const bool pad=event.type==SDL_EVENT_GAMEPAD_BUTTON_DOWN;
    if (!key && !pad) return event.type==SDL_EVENT_KEY_UP ||
        event.type==SDL_EVENT_GAMEPAD_BUTTON_UP || event.type==SDL_EVENT_GAMEPAD_AXIS_MOTION;
    const auto pressed=[&](SDL_Keycode k,SDL_GamepadButton b) {
        return (key && event.key.key==k) || (pad && event.gbutton.button==b);
    };
    if (pressed(SDLK_RETURN,SDL_GAMEPAD_BUTTON_SOUTH)) v.play_requested=true;
    if (pressed(SDLK_R,SDL_GAMEPAD_BUTTON_WEST)) {
        SDL_Gamepad* controller=pad ? SDL_GetGamepadFromID(event.gbutton.which) : nullptr;
        if(controller && SDL_GetGamepadAxis(controller,SDL_GAMEPAD_AXIS_LEFT_TRIGGER)>16000)
            reroll_worldgen_inhabitants(v);
        else { ++v.seed; v.regenerate_requested=true; }
    }
    if(key && !event.key.repeat && event.key.key==SDLK_N)reroll_worldgen_inhabitants(v);
    if (pressed(SDLK_F,SDL_GAMEPAD_BUTTON_NORTH)) fit_worldgen(v);
    if (pressed(SDLK_PAGEUP,SDL_GAMEPAD_BUTTON_LEFT_SHOULDER)) {
        v.floor=v.floor==1 ? 4 : v.floor-1; v.regenerate_requested=true;
    }
    if (pressed(SDLK_PAGEDOWN,SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)) {
        v.floor=v.floor==4 ? 1 : v.floor+1; v.regenerate_requested=true;
    }
    if (key && event.key.key==SDLK_HOME) select_worldgen_checkpoint(v,0);
    if (key && event.key.key==SDLK_END) select_worldgen_checkpoint(v,static_cast<int>(v.trace.checkpoints.size())-1);
    if (pressed(SDLK_LEFTBRACKET,SDL_GAMEPAD_BUTTON_DPAD_LEFT)) select_worldgen_checkpoint(v,v.checkpoint-1);
    if (pressed(SDLK_RIGHTBRACKET,SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) select_worldgen_checkpoint(v,v.checkpoint+1);
    if (key && !event.key.repeat && event.key.key==SDLK_T) {v.capture_options.details=!v.capture_options.details;recapture_worldgen(v);}
    if (pressed(SDLK_MINUS,SDL_GAMEPAD_BUTTON_DPAD_DOWN)) v.zoom=std::max(.08F,v.zoom/1.2F);
    if (pressed(SDLK_EQUALS,SDL_GAMEPAD_BUTTON_DPAD_UP)) v.zoom=std::min(8.0F,v.zoom*1.2F);
    if (pad && event.gbutton.button==SDL_GAMEPAD_BUTTON_START) debug_panels().visible=!debug_panels().visible;
    if (key && event.key.key==SDLK_G) v.rooms=!v.rooms;
    if (pad && event.gbutton.button==SDL_GAMEPAD_BUTTON_LEFT_STICK) v.render.fullbright=!v.render.fullbright;
    if (pressed(SDLK_O,SDL_GAMEPAD_BUTTON_RIGHT_STICK)) v.render.roofs=!v.render.roofs;
    if (key && event.key.key==SDLK_L) v.render.fullbright=!v.render.fullbright;
    if (key && event.key.key==SDLK_V) v.render.overhead=!v.render.overhead;
    if (key && event.key.key==SDLK_C) {
        const std::string recipe=v.original && v.original->generation_report ?
            worldgen_recipe(*v.original->generation_report) : std::to_string(v.seed);
        SDL_SetClipboardText(recipe.c_str());
    }
    if (pressed(SDLK_ESCAPE,SDL_GAMEPAD_BUTTON_EAST)) {
        v.exit_requested=true;
    }
    return true;
}

void update_worldgen(float seconds) {
    auto& v=worldgen_viewer();
    if (!v.active || debug_captures_input()) return;
    const bool* keys=SDL_GetKeyboardState(nullptr);
    float x=static_cast<float>(keys[SDL_SCANCODE_D])-static_cast<float>(keys[SDL_SCANCODE_A]);
    float y=static_cast<float>(keys[SDL_SCANCODE_S])-static_cast<float>(keys[SDL_SCANCODE_W]);
    int count=0;
    SDL_JoystickID* pads=SDL_GetGamepads(&count);
    for (int i=0;i<count;++i) {
        SDL_Gamepad* pad=SDL_GetGamepadFromID(pads[i]);
        if (!pad) continue;
        const auto axis=[&](SDL_GamepadAxis id) {
            const float value=static_cast<float>(SDL_GetGamepadAxis(pad,id))/32767.0F;
            return std::abs(value)<.2F ? 0.0F : value;
        };
        x+=axis(SDL_GAMEPAD_AXIS_LEFTX); y+=axis(SDL_GAMEPAD_AXIS_LEFTY);
    }
    SDL_free(pads);
    const float speed=seconds*80.0F/std::max(v.zoom,.2F);
    v.render.camera.x+=x*speed; v.render.camera.y+=y*speed;
}
