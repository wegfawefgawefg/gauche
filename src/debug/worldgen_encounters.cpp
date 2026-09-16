#include "worldgen.hpp"
#include "../world/forest_encounter_rules.hpp"
#include "generation_build.hpp"
#include <imgui.h>

namespace {
void choices(const char* label,std::span<const WeightedComponent> pool,bool open=false) {
    if(!ImGui::TreeNodeEx(label,open ? ImGuiTreeNodeFlags_DefaultOpen : 0))return;
    unsigned total=0;for(const auto& choice:pool)total+=choice.weight;
    ImGui::Text("Pool total: %u",total);
    if(ImGui::BeginTable("options",3,ImGuiTableFlags_Borders|ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Candidate");ImGui::TableSetupColumn("Weight");ImGui::TableSetupColumn("Eligibility");ImGui::TableHeadersRow();
        for(const auto& choice:pool) {
            ImGui::TableNextRow();ImGui::TableNextColumn();ImGui::TextWrapped("%s",choice.name);
            ImGui::TableNextColumn();ImGui::Text("%u",choice.weight);
            ImGui::TableNextColumn();ImGui::TextUnformatted(choice.weight ? "Eligible" : "Excluded");
        }
        ImGui::EndTable();
    }
    ImGui::TreePop();
}
}

void draw_forest_encounter_pools(const GenerationReport& report) {
    if(!ImGui::CollapsingHeader("Ordinary encounter eligibility (all pools)"))return;
    if(report.received && report.revision!=GAUCHE_GENERATOR_REVISION) {
        ImGui::TextWrapped("Host generator build differs from this client. Potential pools cannot be reconstructed reliably; use the host's recorded choices below.");return;
    }
    if(!forest_floor(report.floor)) {
        ImGui::TextUnformatted("These ordinary encounter pools apply only to Forest.");return;
    }
    static RoomRole role=RoomRole::Clearing;
    ImGui::Text("Forest 1-%d",biome_stage(report.floor));
    ImGui::TextWrapped("Themes: %s / %s",theme_rule(report.themes.major).name,theme_rule(report.themes.minor).name);
    if(ImGui::BeginCombo("Preview room role",room_name(role))) {
        for(int value=0;value<=static_cast<int>(RoomRole::Secret);++value) {
            const auto candidate=static_cast<RoomRole>(value);
            if(ImGui::Selectable(room_name(candidate),role==candidate))role=candidate;
            if(role==candidate)ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::TextWrapped("Potential choices for this floor's rules and themes; not a placement result. Changing the preview role does not change the world. Reserved landmarks and claimed bear clearings use their own populations.");
    const auto rules=forest_encounter_rules(report.floor,role,report.themes);
    if(rules.exclusion){ImGui::TextWrapped("%s",rules.exclusion);return;}
    if(const auto* actual=feature_decision(report,GenerationFeature::ForestEncounters);
        actual && actual->outcome==GenerationOutcome::Suppressed)
        ImGui::TextWrapped("Ordinary encounter generation was suppressed on this floor: %s",actual->reason.c_str());
    ImGui::TextWrapped("Weights compare candidates within one pool, not whole-floor percentages. A zero cannot roll. Available space can reduce group size. Specialists and wildlife have separate slots; they do not spend the pack's fighter allowance.");
    choices("Pack spacing",rules.spacing);
    choices("Requested group size",rules.sizes);
    choices("Pack families",rules.families,true);
    for(std::size_t i=0;i<rules.family_exclusions.size();++i)if(rules.family_exclusions[i])
        ImGui::TextWrapped("%s: %s",rules.families[i].name,rules.family_exclusions[i]);
    if(ImGui::TreeNode("Member pools (conditional on family)")) {
        for(const auto& family:rules.families) {
            const std::string label=std::string(family.name)+(family.weight ? "" : " (family excluded here)");
            choices(label.c_str(),rules.members[static_cast<std::size_t>(family.value)]);
        }
        ImGui::TreePop();
    }
    ImGui::TextWrapped("%s",rules.specialist_note);
    choices("Additional specialists",rules.specialists,true);
    choices("Wildlife / scavengers",rules.wildlife);
}
