#include "worldgen.hpp"
#include "worldgen_sidebar.hpp"
#include "generation_overlay.hpp"
#include "generation_build.hpp"
#include <imgui.h>
#include <algorithm>

void draw_generation_report(const GenerationReport& report,bool inspection) {
    auto& v=worldgen_viewer();
    auto& live=live_generation_inspector();
    int& selected_feature=inspection ? v.selected_feature : live.selected_feature;
    int& selected_component=inspection ? v.selected_component : live.selected_component;
    ImGui::SeparatorText("Generation rules / recorded decisions");
    if(report.geometry_omitted)ImGui::TextWrapped("Map geometry omitted to fit the network diagnostic budget. Decisions/counts are retained; map annotations are unavailable.");
    ImGui::TextWrapped("Base selection chances per ordinary floor. Placement can still fail; selected rows show actual theme-adjusted chances.");
    if (const auto* themes=feature_decision(report,GenerationFeature::Themes);
        themes && themes->outcome!=GenerationOutcome::Suppressed && themes->outcome!=GenerationOutcome::Ineligible) {
        ImGui::TextWrapped("Identity: %s | Modifier: %s",theme_rule(report.themes.major).name,theme_rule(report.themes.minor).name);
    }
    if (ImGui::CollapsingHeader("Theme weights / compatibility")) {
        ImGui::TextWrapped("Conditional slot weights, not floor percentages. A zero cannot roll. Minor weights below include this floor's selected major identity.");
        for (const auto& rule:generation_theme_rules) {
            ImGui::Separator();ImGui::TextUnformatted(rule.name);
            ImGui::Text("Major F1..F4: %u, %u, %u, %u",rule.major[0],rule.major[1],rule.major[2],rule.major[3]);
            ImGui::Text("Minor here: %u",theme_weight(rule,report.floor,true,report.themes.major));
            if (!themes_compatible(rule.theme,report.themes.major)) ImGui::TextWrapped("Excluded with %s",theme_rule(report.themes.major).name);
            else if (!biome_floor(report.floor,rule.biome)) ImGui::TextUnformatted("Outside registered biome");
        }
    }
    draw_forest_encounter_pools(report);
    if (ImGui::CollapsingHeader("Stage eligibility (1-1 through 1-4)"))
    if (ImGui::BeginTable("eligibility",5,ImGuiTableFlags_Borders|ImGuiTableFlags_SizingStretchSame)) {
        ImGui::TableSetupColumn("Feature");
        for (const char* title:{"1-1","1-2","1-3","1-4"}) ImGui::TableSetupColumn(title);
        ImGui::TableHeadersRow();
        for (const auto& rule:generation_rules) {
            ImGui::TableNextRow();ImGui::TableNextColumn();ImGui::TextWrapped("%s",rule.name);
            for (int floor=1;floor<=4;++floor) {
                ImGui::TableNextColumn();
                const auto denominator=feature_denominator(rule,floor);
                if (denominator) ImGui::Text("1/%u",denominator);
                else ImGui::TextUnformatted("--");
            }
        }
        ImGui::EndTable();
    }
    static int filter=0;
    ImGui::Combo("Show",&filter,"All registered\0Eligible here\0Reserved / built\0");
    for (const auto& rule:generation_rules) {
        const auto* decision=feature_decision(report,rule.feature);
        const auto outcome=decision ? decision->outcome : GenerationOutcome::Pending;
        const bool built=outcome==GenerationOutcome::Built || outcome==GenerationOutcome::Reserved;
        if (filter==1 && !feature_denominator(rule,report.floor)) continue;
        if (filter==2 && !built) continue;
        const int id=static_cast<int>(rule.feature);
        ImGui::PushID(id);
        const std::string label=std::string{rule.name}+" - "+generation_outcome_name(outcome);
        if (ImGui::Selectable(label.c_str(),selected_feature==id)) {selected_feature=id;selected_component=-1;}
        if (selected_feature==id) {
            if (!decision) ImGui::TextWrapped("This planner has not run at this checkpoint.");
            else {
                ImGui::TextWrapped("%s",decision->reason.c_str());
                if (decision->denominator && outcome!=GenerationOutcome::Suppressed && rule.feature!=GenerationFeature::Themes && rule.feature!=GenerationFeature::RoomComposition)
                    ImGui::Text("Roll %u of [0,%u); 0 selects",decision->roll,decision->denominator);
                if (rule.feature==GenerationFeature::RoomComposition && decision->denominator)
                    ImGui::TextUnformatted("Required planning pass; assignments do not consume extra rolls");
                const auto base=feature_denominator(rule,report.floor);
                if (base && decision->denominator && base!=decision->denominator)
                    ImGui::Text("Actual 1/%u; base 1/%u before theme",decision->denominator,base);
                if (decision->candidate_count>=0) {
                    const bool block=rule.feature==GenerationFeature::GiantTree || rule.feature==GenerationFeature::TimberGrove;
                    ImGui::TextWrapped("%d %s",decision->candidate_count,block ? "weighted block entries (weighted by existing corners)" : rule.feature==GenerationFeature::ForestEncounters ? "eligible ordinary rooms" : rule.feature==GenerationFeature::RoomComposition ? "planned rooms" : "eligible placement candidates");
                }
                if (!decision->variant.empty()) ImGui::TextWrapped("Variant: %s",decision->variant.c_str());
                if (!decision->regions.empty()) {
                    ImGui::Text("%zu footprints",decision->regions.size());
                    ImGui::BeginDisabled(!inspection);
                    if (ImGui::Button("Focus footprint")) {
                        focus_worldgen_selection(v,report,false);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("First appearance pass")) {
                        for (std::size_t i=0;i<v.trace.checkpoints.size();++i) {
                            const auto* earlier=feature_decision(v.trace.checkpoints[i].report,rule.feature);
                            if (earlier && (earlier->outcome==GenerationOutcome::Reserved || earlier->outcome==GenerationOutcome::Built)) {select_worldgen_checkpoint(v,static_cast<int>(i));break;}
                        }
                    }
                    ImGui::EndDisabled();
                }
                draw_component_decisions(report,rule.feature,inspection);
            }
        }
        ImGui::PopID();
    }
    ImGui::TextWrapped("Coverage: floor identities, Forest room roles and reassignment sources, landmarks, sectors, rivers and ordinary encounters. Later prop/actor removal attribution remains partial.");
}

void draw_live_generation_details(const Game& game) {
    auto& v=worldgen_viewer();
    auto& live=bind_live_generation_report(game);
    ImGui::SetNextWindowSize({420,570},ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Generation inspector",&v.details)) {
        if (game.generation_report) {
            const auto& report=*game.generation_report;
            ImGui::Text("Current floor %d | run seed %llu",report.floor,static_cast<unsigned long long>(report.seed));
            ImGui::Text("Planner RNG %llu",static_cast<unsigned long long>(report.initial_rng));
            if(report.inhabitants_seed)ImGui::Text("Inhabitants seed %llu (after boss geometry)",static_cast<unsigned long long>(report.inhabitants_seed));
            const char* revision=report.revision.empty() ? GAUCHE_GENERATOR_REVISION : report.revision.c_str();
            ImGui::TextWrapped("Generator build: %s",revision);
            if(ImGui::Button("Copy floor diagnostics")) {
                const std::string text=worldgen_recipe(report);
                SDL_SetClipboardText(text.c_str());
            }
            ImGui::Checkbox("Show recorded map annotations",&live.overlay);
            if(ImGui::CollapsingHeader("Annotation layers"))draw_generation_annotation_controls(live.annotations);
            ImGui::SameLine();
            if(ImGui::Button("Clear selection"))live.selected_feature=live.selected_component=-1;
            ImGui::TextWrapped("Select a feature or child roll below. Gold: feature bounds; cyan: area outline or site crosses plus guide; pink: empty attempt; orange: rejected cells. Marks stay at generation-time positions, even after actors move or scenery breaks. F1 hides the panel; the enabled overlay remains visible.");
            ImGui::TextWrapped("%s Decisions describe generation, not what is still alive.",report.received ? "Recorded by the host and received with this world snapshot." : "Recorded when this floor was generated locally.");
            draw_generation_report(report,false);
        } else ImGui::TextWrapped("No generation report for this state. The host may have developer diagnostics disabled, or the attachment was unavailable. This window never substitutes an unrelated preview.");
    }
    ImGui::End();
}
