#include "worldgen.hpp"
#include <imgui.h>
#include <algorithm>

void draw_generation_report(const GenerationReport& report,bool inspection) {
    auto& v=worldgen_viewer();
    ImGui::SeparatorText("Generation rules / recorded decisions");
    ImGui::TextWrapped("Initial selection chances per ordinary floor. Placement can still fail. Rows are in generator order.");
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
        if (ImGui::Selectable(label.c_str(),v.selected_feature==id)) {v.selected_feature=id;v.selected_component=-1;}
        if (v.selected_feature==id) {
            if (!decision) ImGui::TextWrapped("This planner has not run at this checkpoint.");
            else {
                ImGui::TextWrapped("%s",decision->reason.c_str());
                if (decision->denominator && outcome!=GenerationOutcome::Suppressed)
                    ImGui::Text("Roll %u of [0,%u); 0 selects",decision->roll,decision->denominator);
                if (decision->candidate_count>=0) {
                    const bool block=rule.feature==GenerationFeature::GiantTree || rule.feature==GenerationFeature::TimberGrove;
                    ImGui::TextWrapped("%d %s",decision->candidate_count,block ? "weighted block entries (weighted by existing corners)" : "eligible adjacent room pairs");
                }
                if (!decision->variant.empty()) ImGui::TextWrapped("Variant: %s",decision->variant.c_str());
                if (!decision->regions.empty()) {
                    ImGui::Text("%zu footprints",decision->regions.size());
                    ImGui::BeginDisabled(!inspection);
                    if (ImGui::Button("Focus footprint")) {
                        Cell low=decision->regions.front().low,high=decision->regions.front().high;
                        for (const auto& region:decision->regions) {
                            low.x=std::min(low.x,region.low.x);low.y=std::min(low.y,region.low.y);
                            high.x=std::max(high.x,region.high.x);high.y=std::max(high.y,region.high.y);
                        }
                        v.render.camera={static_cast<float>(low.x+high.x)*.5F,static_cast<float>(low.y+high.y)*.5F};
                        v.zoom=std::min(400.0F/(8.0F*static_cast<float>(high.x-low.x+4)),
                                        240.0F/(8.0F*static_cast<float>(high.y-low.y+4)));
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("First appearance pass")) {
                        for (std::size_t i=0;i<v.trace.checkpoints.size();++i) {
                            const auto* earlier=feature_decision(v.trace.checkpoints[i].report,rule.feature);
                            if (earlier && (earlier->outcome==GenerationOutcome::Reserved || earlier->outcome==GenerationOutcome::Built)) {v.checkpoint=static_cast<int>(i);break;}
                        }
                    }
                    ImGui::EndDisabled();
                }
                draw_component_decisions(report,rule.feature,inspection);
            }
        }
        ImGui::PopID();
    }
    ImGui::TextWrapped("Coverage: six large Forest landmarks, open sectors and rivers, plus their recorded child rolls. Ordinary room-role rolls and later removals remain untraced.");
}

void draw_live_generation_details(const Game& game) {
    auto& v=worldgen_viewer();
    ImGui::SetNextWindowSize({420,570},ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Generation inspector",&v.details)) {
        if (game.generation_report) {
            const auto& report=*game.generation_report;
            ImGui::Text("Current floor %d | run seed %llu",report.floor,static_cast<unsigned long long>(report.seed));
            ImGui::Text("Planner RNG %llu",static_cast<unsigned long long>(report.initial_rng));
            ImGui::TextWrapped("Recorded when this floor was generated locally. Decisions survive gameplay; they are not a census of what is still alive.");
            draw_generation_report(report,false);
        } else ImGui::TextWrapped("No local generation report for this state. Received network snapshots do not carry diagnostics yet; this window will not substitute an unrelated preview.");
    }
    ImGui::End();
}
