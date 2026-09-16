#include "worldgen.hpp"
#include "generation_build.hpp"
#include "../input.hpp"
#include <imgui.h>
#include <algorithm>
#include <cstdio>

namespace {
void room_box(SDL_Renderer* renderer,const RoomPlan& room,const WorldGenViewer& v) {
    SDL_FRect box=tile_rect(room.center-Cell{room.half_width,room.half_height},v.render.camera,v.zoom);
    box.w=static_cast<float>(room.half_width*2+1)*tile_pixels(v.zoom);
    box.h=static_cast<float>(room.half_height*2+1)*tile_pixels(v.zoom);
    SDL_SetRenderDrawColor(renderer,room.landmark ? 255 : 120,210,room.landmark ? 90 : 255,190);
    SDL_RenderRect(renderer,&box);
}
}

void draw_worldgen(SDL_Renderer* renderer,const GameGraphics& graphics) {
    const auto& v=worldgen_viewer();
    if (!v.original || v.trace.checkpoints.empty()) return;
    const auto& selected=v.trace.checkpoints[static_cast<std::size_t>(v.checkpoint)];
    const Game& game=*selected.game;
    if (game.stage.width>0)
        render_game(renderer,graphics,game,0,v.zoom,nullptr,PointerState{},false,false,&v.render);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    if (v.rooms) for (const auto& room:selected.rooms) room_box(renderer,room,v);
    if (v.selected_feature>=0) {
        const auto* decision=feature_decision(selected.report,static_cast<GenerationFeature>(v.selected_feature));
        if (decision) for (const auto& region:decision->regions) {
            auto box=tile_rect(region.low,v.render.camera,v.zoom);
            box.w=static_cast<float>(region.high.x-region.low.x)*tile_pixels(v.zoom);
            box.h=static_cast<float>(region.high.y-region.low.y)*tile_pixels(v.zoom);
            SDL_SetRenderDrawColor(renderer,255,210,80,65);SDL_RenderFillRect(renderer,&box);
            SDL_SetRenderDrawColor(renderer,255,230,90,255);SDL_RenderRect(renderer,&box);
        }
    }
    if (v.selected_component>=0 && static_cast<std::size_t>(v.selected_component)<selected.report.components.size()) {
        const auto& child=selected.report.components[static_cast<std::size_t>(v.selected_component)];
        SDL_SetRenderDrawColor(renderer,child.placed ? 90 : 255,child.placed ? 255 : 100,210,255);
        const auto mark=[&](Cell cell) {
            const auto box=tile_rect(cell,v.render.camera,v.zoom);
            SDL_RenderLine(renderer,box.x,box.y,box.x+box.w,box.y+box.h);
            SDL_RenderLine(renderer,box.x+box.w,box.y,box.x,box.y+box.h);
        };
        if (child.cells.empty()) mark(child.anchor);
        else for (Cell cell:child.cells) {
            if (child.guide.empty()) mark(cell);
            else {
                SDL_SetRenderDrawColor(renderer,90,240,210,100);
                const auto box=tile_rect(cell,v.render.camera,v.zoom);SDL_RenderFillRect(renderer,&box);
            }
        }
        SDL_SetRenderDrawColor(renderer,255,140,90,190);
        for (Cell cell:child.rejected_cells) mark(cell);
        SDL_SetRenderDrawColor(renderer,child.placed ? 90 : 255,child.placed ? 255 : 100,210,255);
        for (std::size_t i=1;!child.guide.empty() && i<child.guide.size()+(child.guide_closed ? 1U : 0U);++i) {
            const auto a=tile_rect(child.guide[i-1],v.render.camera,v.zoom);
            const auto b=tile_rect(child.guide[i%child.guide.size()],v.render.camera,v.zoom);
            const float offset=child.guide_closed ? 0.0F : a.w*.5F;
            SDL_RenderLine(renderer,a.x+offset,a.y+offset,b.x+offset,b.y+offset);
        }
    }
    if ((v.changes || v.actor_changes) && v.checkpoint>0)
        draw_worldgen_changes(renderer,v,*v.trace.checkpoints[static_cast<std::size_t>(v.checkpoint-1)].game,game);
    SDL_SetRenderDrawColor(renderer,10,14,18,240);
    const SDL_FRect top{0,0,640,24},bottom{0,321,640,39};
    SDL_RenderFillRect(renderer,&top); SDL_RenderFillRect(renderer,&bottom);
    SDL_SetRenderDrawColor(renderer,235,235,215,255);
    char title[160];
    std::snprintf(title,sizeof(title),"FOREST 1-%d | seed %llu | step %d/%zu: %s",v.original->run.floor,
        static_cast<unsigned long long>(v.original->run.seed),v.checkpoint+1,v.trace.checkpoints.size(),selected.name.c_str());
    SDL_RenderDebugText(renderer,8,8,title);
    SDL_RenderDebugText(renderer,8,327,"A/Enter Play  B/Esc Exit  X/R Regen  Y/F Fit  LB/RB Floor");
    SDL_RenderDebugText(renderer,8,338,"Stick/WASD Pan  D-pad Up/Down Zoom  Left/Right Step  Start/F1 Details");
    SDL_RenderDebugText(renderer,8,349,"F6/Back Return | O Roofs  L Light  V Vignette  T Fine  C Copy seed");
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
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
            const std::string recipe="Forest 1-"+std::to_string(v.original->run.floor)+
                " | seed "+std::to_string(v.original->run.seed)+
                " | revision " GAUCHE_GENERATOR_REVISION " | standalone / automatic / default kit"+
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
        if (ImGui::BeginTabItem("Capture")) {
            ImGui::Checkbox("Fine steps (next capture)",&v.capture_options.details);
            const int ids[]{-1,static_cast<int>(GenerationFeature::OpenSectors),static_cast<int>(GenerationFeature::River),static_cast<int>(GenerationFeature::SpiderCave),static_cast<int>(GenerationFeature::GiantTree)};
            const char* labels[]{"All instrumented features","Sector attempts","River attempts","Spider branches","Giant-tree roots"};
            int current=0;for (int i=1;i<5;++i) if (ids[i]==v.capture_options.feature) current=i;
            if (ImGui::Combo("Fine scope",&current,labels,5)) v.capture_options.feature=ids[current];
            ImGui::SliderInt("Every N attempts",&v.capture_options.every,1,16);
            if (ImGui::Button("Recapture same map")) recapture_worldgen(v);
            ImGui::TextWrapped("Same seed/floor and camera. Capture changes inspection only. T toggles fine capture. Other generator loops remain coarse.");
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
