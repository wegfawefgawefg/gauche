#include "worldgen.hpp"
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
    if (v.changes && v.checkpoint>0) {
        const auto& before=v.trace.checkpoints[static_cast<std::size_t>(v.checkpoint-1)].game->stage;
        for (int y=0;y<game.stage.height;++y) for (int x=0;x<game.stage.width;++x) {
            const Cell cell{x,y}; const Tile& now=*game.stage.at(cell);
            const Tile* old=before.at(cell);
            if (old && old->kind==now.kind && old->material==now.material && old->prop.kind==now.prop.kind) continue;
            SDL_SetRenderDrawColor(renderer,255,130,40,95);
            const auto box=tile_rect(cell,v.render.camera,v.zoom); SDL_RenderFillRect(renderer,&box);
        }
    }
    SDL_SetRenderDrawColor(renderer,10,14,18,240);
    const SDL_FRect top{0,0,640,24},bottom{0,321,640,39};
    SDL_RenderFillRect(renderer,&top); SDL_RenderFillRect(renderer,&bottom);
    SDL_SetRenderDrawColor(renderer,235,235,215,255);
    char title[160];
    std::snprintf(title,sizeof(title),"FOREST 1-%d | seed %llu | pass %d/%zu: %s",v.original->run.floor,
        static_cast<unsigned long long>(v.original->run.seed),v.checkpoint+1,v.trace.checkpoints.size(),selected.name.c_str());
    SDL_RenderDebugText(renderer,8,8,title);
    SDL_RenderDebugText(renderer,8,327,"A/Enter Play  B/Esc Exit  X/R Regen  Y/F Fit  LB/RB Floor");
    SDL_RenderDebugText(renderer,8,338,"Stick/WASD Pan  D-pad Up/Down Zoom  Left/Right Pass  Start/F1 Details");
    SDL_RenderDebugText(renderer,8,349,"F6/Back: return from play | O Roofs  L Lighting  V Vignette  C Copy seed");
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
}

void draw_worldgen_details() {
    auto& v=worldgen_viewer();
    if (!v.original || (!v.active && !v.details)) return;
    ImGui::SetNextWindowPos({16,100},ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({350,480},ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Generation inspector",&v.details)) {
        ImGui::TextWrapped("Revision: %s", GAUCHE_GENERATOR_REVISION);
        if (ImGui::Button("Copy generation recipe")) {
            const std::string recipe="Forest 1-"+std::to_string(v.original->run.floor)+
                " | seed "+std::to_string(v.original->run.seed)+
                " | revision " GAUCHE_GENERATOR_REVISION " | standalone / automatic / default kit";
            SDL_SetClipboardText(recipe.c_str());
        }
        ImGui::TextUnformatted("Standalone floor / automatic layout / default kit");
        if (!v.active) ImGui::TextWrapped("Retained inspected floor, not a live network generation report.");
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
        const int last=std::max(0,static_cast<int>(v.trace.checkpoints.size())-1);
        ImGui::SliderInt("Pass",&v.checkpoint,0,last);
        if (ImGui::BeginListBox("##passes",{-1,130})) {
            for (int i=0;i<=last;++i) {
                const auto& step=v.trace.checkpoints[static_cast<std::size_t>(i)];
                if (ImGui::Selectable(step.name.c_str(),v.checkpoint==i)) v.checkpoint=i;
            }
            ImGui::EndListBox();
        }
        ImGui::EndDisabled();
        if (v.trace.truncated) ImGui::TextUnformatted("Checkpoint limit reached; Play still uses finished map.");
        ImGui::SeparatorText("Recorded population results (finished map)");
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
        ImGui::TextWrapped("Eligibility and unselected rolls are not captured yet. These counts describe actual generated content only.");
    }
    ImGui::End();
}
