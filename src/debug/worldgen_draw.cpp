#include "worldgen.hpp"
#include "worldgen_sidebar.hpp"
#include "generation_overlay.hpp"
#include "ambient_inspector.hpp"
#include "generation_build.hpp"
#include "../input.hpp"
#include <imgui.h>
#include <algorithm>
#include <cstdio>

namespace {
void room_box(tr::Renderer* renderer,const RoomPlan& room,const WorldGenViewer& v) {
    SDL_FRect box=tile_rect(room.center-Cell{room.half_width,room.half_height},v.render.camera,v.zoom);
    box.w=static_cast<float>(room.half_width*2+1)*tile_pixels(v.zoom);
    box.h=static_cast<float>(room.half_height*2+1)*tile_pixels(v.zoom);
    tr::set_color_bytes(renderer,room.landmark ? 255 : 120,210,room.landmark ? 90 : 255,190);
    tr::rect(renderer,&box);
    if(v.room_labels) {
        const std::string label=std::string(room_name(room.role))+" / "+std::to_string(room.turns*90)+"deg"+(room.mirrored ? " mirrored" : "");
        tr::debug_text(renderer,box.x+2,box.y+2,label.c_str());
    }
}
}

void draw_worldgen(tr::Renderer* renderer,const GameGraphics& graphics) {
    const auto& v=worldgen_viewer();
    if (!v.original || v.trace.checkpoints.empty()) return;
    const auto& selected=v.trace.checkpoints[static_cast<std::size_t>(v.checkpoint)];
    const Game& game=*selected.game;
    if (game.stage.width>0)
        render_game(renderer,graphics,game,0,v.zoom,nullptr,PointerState{},false,false,&v.render);
    tr::set_blend(renderer,SDL_BLENDMODE_BLEND);
    if (v.rooms) for (const auto& room:selected.rooms) room_box(renderer,room,v);
    draw_generation_annotations(renderer,selected.report,v.selected_feature,v.selected_component,v.render.camera,v.zoom,v.annotations);
    draw_ambient_annotations(renderer,game,v.render.camera,v.zoom);
    if ((v.changes || v.actor_changes) && v.checkpoint>0)
        draw_worldgen_changes(renderer,v,*v.trace.checkpoints[static_cast<std::size_t>(v.checkpoint-1)].game,game);
    draw_worldgen_sidebar(renderer,v,selected.report);
    tr::set_color_bytes(renderer,10,14,18,240);
    const SDL_FRect top{0,0,640,24},bottom{0,321,640,39};
    tr::fill_rect(renderer,&top); tr::fill_rect(renderer,&bottom);
    tr::set_color_bytes(renderer,235,235,215,255);
    char title[160];
    std::snprintf(title,sizeof(title),"FOREST 1-%d | seed %llu | step %d/%zu: %s",v.original->run.floor,
        static_cast<unsigned long long>(v.original->run.seed),v.checkpoint+1,v.trace.checkpoints.size(),selected.name.c_str());
    if(v.original->generation_report && v.original->generation_report->inhabitants_seed) {
        std::snprintf(title,sizeof(title),"FOREST 1-%d | seed %llu | inhabitants %llu",v.original->run.floor,
            static_cast<unsigned long long>(v.original->run.seed),
            static_cast<unsigned long long>(v.original->generation_report->inhabitants_seed));
        tr::debug_text(renderer,8,3,title);
        std::snprintf(title,sizeof(title),"step %d/%zu: %s",v.checkpoint+1,v.trace.checkpoints.size(),selected.name.c_str());
        tr::debug_text(renderer,8,14,title);
    } else tr::debug_text(renderer,8,8,title);
    tr::debug_text(renderer,8,327,v.sidebar_focus ? "D-pad/Arrows: rolls  A/Enter: focus  B/Back/Tab: map  X/R: regen" : "A/Enter Play  B/Esc Exit  X/R Regen  Y/F Fit  LB/RB Floor");
    tr::debug_text(renderer,8,338,v.sidebar_focus ? "Right: child rolls  Left: features  Y/F Fit  LB/RB Floor  Start/F1 Details" : "Stick/WASD Pan  D-pad Up/Down Zoom  Left/Right Step  Start/F1 Details");
    tr::debug_text(renderer,8,349,"Tab/Back Rolls | LT+X/N Later rolls | O Roofs L Light V Shade C Recipe");
    tr::set_blend(renderer,SDL_BLENDMODE_NONE);
}

void draw_worldgen_details(const Game& live_game) {
    auto& v=worldgen_viewer();
    if (!v.active) { draw_live_generation_details(live_game); return; }
    if (!v.original) return;
    ImGui::SetNextWindowPos({16,100},ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({380,540},ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Generation inspector",&v.details)) {
        ImGui::TextWrapped("Revision: %s", GAUCHE_GENERATOR_REVISION);
        if (ImGui::Button("Copy generation recipe")) {
            const std::string recipe=worldgen_recipe(*v.original->generation_report)+
                " | standalone / automatic / default kit"+
                " | fine "+std::to_string(v.trace.options.details)+" | feature "+std::to_string(v.trace.options.feature)+
                " | every "+std::to_string(v.trace.options.every);
            SDL_SetClipboardText(recipe.c_str());
        }
        ImGui::TextWrapped("Standalone floor / automatic layout / default kit");
        if (ImGui::BeginTabBar("generation-sections")) {
        if (ImGui::BeginTabItem("View / timeline")) {
        ImGui::BeginDisabled(!v.active);
        ImGui::InputScalar("Next seed",ImGuiDataType_U64,&v.seed);
        if (ImGui::Button("Generate seed")) v.regenerate_requested=true;
        ImGui::SameLine();
        if (ImGui::Button("Copy seed")) SDL_SetClipboardText(std::to_string(v.original->run.seed).c_str());
        if (ImGui::SliderInt("Forest floor",&v.floor,1,4)) v.regenerate_requested=true;
        if (ImGui::Button("Play finished map")) v.play_requested=true;
        ImGui::SameLine(); if (ImGui::Button("Fit")) fit_worldgen(v);
        ImGui::Checkbox("Fullbright",&v.render.fullbright);
        ImGui::SameLine(); ImGui::Checkbox("Roof / crown",&v.render.roofs);
        ImGui::Checkbox("Vignette",&v.render.overhead);
        ImGui::SameLine(); ImGui::Checkbox("Room bounds",&v.rooms);
        ImGui::Checkbox("Changed terrain / props",&v.changes);
        ImGui::Checkbox("Changed actors / loot",&v.actor_changes);
        if (v.actor_changes) ImGui::TextWrapped("Green: added. Red: removed/old position. Yellow: changed/moved. Compared with the preceding captured step.");
        ImGui::Checkbox("Follow fine-step component",&v.follow_step);
        ImGui::Text("%zu pass + %zu fine snapshots; about %.1f MiB",v.trace.coarse_count,v.trace.detail_count,static_cast<double>(v.trace.bytes)/(1024*1024));
        const int last=std::max(0,static_cast<int>(v.trace.checkpoints.size())-1);
        if (ImGui::Button("First step")) select_worldgen_checkpoint(v,0);
        ImGui::SameLine();if (ImGui::Button("Finished step")) select_worldgen_checkpoint(v,last);
        int selected_step=v.checkpoint;
        if (ImGui::SliderInt("Step",&selected_step,0,last)) select_worldgen_checkpoint(v,selected_step);
        if (ImGui::BeginListBox("##passes",{-1,130})) {
            for (int i=0;i<=last;++i) {
                const auto& step=v.trace.checkpoints[static_cast<std::size_t>(i)];
                if (ImGui::Selectable(step.name.c_str(),v.checkpoint==i)) select_worldgen_checkpoint(v,i);
            }
            ImGui::EndListBox();
        }
        ImGui::EndDisabled();
        if (v.trace.truncated) ImGui::TextUnformatted("Coarse checkpoint limit reached; Play still uses finished map.");
        if (v.trace.detail_truncated) ImGui::TextWrapped("Fine capture limit reached (32 snapshots / approximately 48 MiB). Later coarse passes are retained. Narrow the scope or increase N to inspect further.");
        if (v.trace.options.details && !v.trace.detail_seen) ImGui::TextWrapped("No instrumented attempts matched this capture's scope.");
        ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Annotations")) {
            ImGui::Checkbox("Room boxes",&v.rooms);
            ImGui::Checkbox("Room role / rotation labels",&v.room_labels);
            draw_generation_annotation_controls(v.annotations);
            if(ImGui::Button("Inspect ambient sounds"))ambient_inspector().visible=true;
            ImGui::TextWrapped("Ambient preview derives sources from the selected checkpoint. It does not play or consume triggers.");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Composition")) {
            const auto used=v.original->generation_report ? v.original->generation_report->inhabitants_seed : 0;
            if(used)ImGui::Text("Applied inhabitants seed: %llu",static_cast<unsigned long long>(used));
            else ImGui::TextUnformatted("Applied: ordinary planner stream");
            const bool ordinary=v.original->run.layout==FloorLayout::Generated;
            ImGui::BeginDisabled(!ordinary);
            ImGui::InputScalar("Inhabitants seed",ImGuiDataType_U64,&v.inhabitants_seed);
            if(ImGui::Button("Apply to this terrain")) {
                const auto requested=v.inhabitants_seed;recapture_worldgen(v);v.inhabitants_seed=requested;
            }
            if(ImGui::Button("Next composition (N / LT+X)"))reroll_worldgen_inhabitants(v);
            if(ImGui::Button("Restore planner stream")){recapture_worldgen(v);v.inhabitants_seed=0;}
            ImGui::EndDisabled();
            ImGui::TextWrapped("Keeps the current floor and layout seed through the boss-geometry checkpoint. Rerolls inhabitants, loot and every later pass, including trees, roof scenes and border scenery. Those later passes can still change terrain. Camera stays put; Play uses the finished variant.");
            ImGui::TextWrapped("Zero restores the default stream. Copy recipe includes both seeds. Recapture same map preserves this variant. X/R changes the layout seed; LT+X or N changes only the later rolls.");
            if(!ordinary)ImGui::TextWrapped("This whole-floor unique owns its population pipeline; the override does not apply.");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Capture")) {
            if(ImGui::Checkbox("Results sidebar",&v.sidebar_visible) && !v.sidebar_visible)v.sidebar_focus=false;
            ImGui::Checkbox("Fine steps (next capture)",&v.capture_options.details);
            const char* scope="All instrumented features";
            for(const auto& rule:generation_rules)
                if(rule.fine_steps && static_cast<int>(rule.feature)==v.capture_options.feature)scope=rule.name;
            if(ImGui::BeginCombo("Fine scope",scope)) {
                if(ImGui::Selectable("All instrumented features",v.capture_options.feature<0))v.capture_options.feature=-1;
                for(const auto& rule:generation_rules)if(rule.fine_steps) {
                    const int id=static_cast<int>(rule.feature);
                    if(ImGui::Selectable(rule.name,v.capture_options.feature==id))v.capture_options.feature=id;
                }
                ImGui::EndCombo();
            }
            ImGui::SliderInt("Every N attempts",&v.capture_options.every,1,16);
            if (ImGui::Button("Recapture same map")) recapture_worldgen(v);
            ImGui::TextWrapped("Same layout/inhabitants seeds, floor and camera. Capture changes inspection only. T toggles fine capture. Other generator loops remain coarse.");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Rolls")) {
            draw_generation_report(v.trace.checkpoints[static_cast<std::size_t>(v.checkpoint)].report,true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Population")) {
        ImGui::SeparatorText("Recorded population results (finished map)");
        if (v.original->generation_report) {
            const auto* packs=feature_decision(*v.original->generation_report,GenerationFeature::ForestEncounters);
            if (packs && packs->outcome==GenerationOutcome::Built) {
                ImGui::TextWrapped("%s",packs->variant.c_str());
                ImGui::TextWrapped("Ordinary packs only; landmark populations, wildlife and specialists are separate. See Rolls for member placements and failures.");
            }
        }
        ImGui::Text("Giant trees %zu | Timber groves %zu",v.population.giant_trees.size(),v.population.timber_groves.size());
        ImGui::Text("Snake tunnels %zu | Spider caves %zu",v.population.snake_tunnels.size(),v.population.spider_caves.size());
        ImGui::Text("Root mazes %zu | Bear dens %zu",v.population.root_mazes.size(),v.population.forest_dens.size());
        ImGui::Text("Bear clearings %zu | Fishing bears %zu",v.population.bear_clearings.size(),v.population.bear_fishers.size());
        for (std::size_t i=0;i<v.population.scenes.size();++i) {
            const auto& s=v.population.scenes[i];
            if (!s.planned && !s.attempted && !s.placed) continue;
            ImGui::Text("%s: %d planned, %d placed",room_name(static_cast<RoomRole>(i)),s.planned,s.placed);
            if (s.rejected || s.budget_blocked || s.fallbacks)
                ImGui::Text("  rejected %d | budget %d | fallback %d",s.rejected,s.budget_blocked,s.fallbacks);
        }
        ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
        }
    }
    ImGui::End();
}
