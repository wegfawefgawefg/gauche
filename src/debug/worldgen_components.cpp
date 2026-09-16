#include "worldgen.hpp"
#include "generation_overlay.hpp"
#include <imgui.h>
#include <algorithm>

namespace {
void component_tree(const GenerationReport& report,GenerationFeature feature,int parent,bool inspection,int depth) {
    if (depth>8) return;
    auto& viewer=worldgen_viewer();
    int& selected_component=inspection ? viewer.selected_component : live_generation_inspector().selected_component;
    for (std::size_t i=0;i<report.components.size();++i) {
        const auto& component=report.components[i];
        if (component.feature!=feature || component.parent!=parent) continue;
        ImGui::PushID(static_cast<int>(i));
        const std::string label=component.slot+": "+component.choice;
        const bool expanded=ImGui::TreeNodeEx(label.c_str(),ImGuiTreeNodeFlags_OpenOnArrow|
            (selected_component==static_cast<int>(i) ? ImGuiTreeNodeFlags_Selected : 0));
        if (ImGui::IsItemClicked()) selected_component=static_cast<int>(i);
        if (expanded) {
            ImGui::TextWrapped("%s | %d recorded cells",component.result.c_str(),component.placed);
            if (!component.rejected_cells.empty()) ImGui::Text("%zu skipped cells (orange crosses)",component.rejected_cells.size());
            ImGui::Text("Weighted ticket %u / %u",component.ticket,component.total);
            for (const auto& option:component.options)
                ImGui::TextWrapped("weight %u: %s",option.weight,option.name.c_str());
            ImGui::BeginDisabled(!inspection);
            if (ImGui::Button("Focus site")) {
                viewer.render.camera=component.anchor;viewer.zoom=2.0F;
                if (!component.guide.empty()) {
                    Cell low=component.guide.front(),high=low;
                    for (Cell point:component.guide) {
                        low.x=std::min(low.x,point.x);low.y=std::min(low.y,point.y);
                        high.x=std::max(high.x,point.x);high.y=std::max(high.y,point.y);
                    }
                    viewer.render.camera={static_cast<float>(low.x+high.x)*.5F,static_cast<float>(low.y+high.y)*.5F};
                    viewer.zoom=std::min(400.0F/(8.0F*static_cast<float>(high.x-low.x+4)),240.0F/(8.0F*static_cast<float>(high.y-low.y+4)));
                }
                selected_component=static_cast<int>(i);
            }
            ImGui::EndDisabled();
            component_tree(report,feature,static_cast<int>(i),inspection,depth+1);
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}
}
void draw_component_decisions(const GenerationReport& report,GenerationFeature feature,bool inspection) {
    if (!ImGui::CollapsingHeader("Child component rolls",ImGuiTreeNodeFlags_DefaultOpen)) return;
    ImGui::TextWrapped("Weights belong to each displayed choice pool, not to unconditional floor chances. Empty slots and failed placements are retained.");
    component_tree(report,feature,-1,inspection,0);
    if (report.components_truncated) ImGui::TextWrapped("Component trace reached its %zu-entry cap; generation continued normally.",generation_component_limit);
}
