#include "components.hpp"
#include <algorithm>

ComponentRoll roll_component(Game& game,GenerationReport* report,GenerationFeature feature,
    int parent,const char* slot,Cell anchor,std::span<const WeightedComponent> choices) {
    unsigned total=0;for (const auto& choice:choices) total+=choice.weight;
    if (!total) return {};
    const unsigned ticket=random_u32(game)%total;
    unsigned cursor=ticket;
    const WeightedComponent* selected=nullptr;
    for (const auto& choice:choices) {
        if (cursor<choice.weight) {selected=&choice;break;}
        cursor-=choice.weight;
    }
    if (!selected) return {};
    ComponentRoll result{selected->value,-1};
    if (report && report->components.size()<generation_component_limit) {
        result.record=static_cast<int>(report->components.size());
        ComponentDecision decision;
        decision.feature=feature;decision.parent=parent;decision.slot=slot;decision.anchor=anchor;
        decision.ticket=ticket;decision.total=total;decision.choice=selected->name;
        decision.result="Selected; placement pending";
        for (const auto& choice:choices) decision.options.push_back({choice.value,choice.name,choice.weight});
        report->components.push_back(std::move(decision));
    } else if (report) report->components_truncated=true;
    return result;
}

void component_result(GenerationReport* report,ComponentRoll roll,const char* result,std::span<const Cell> cells) {
    if (!report || roll.record<0 || static_cast<std::size_t>(roll.record)>=report->components.size()) return;
    auto& decision=report->components[static_cast<std::size_t>(roll.record)];
    decision.cells_are_area=false;
    decision.result=result;decision.placed=static_cast<int>(cells.size());
    decision.cells.assign(cells.begin(),cells.end());
}

void component_area(GenerationReport* report,ComponentRoll roll,const char* result,std::span<const Cell> cells) {
    component_result(report,roll,result,cells);
    if(!report || roll.record<0 || static_cast<std::size_t>(roll.record)>=report->components.size())return;
    auto& decision=report->components[static_cast<std::size_t>(roll.record)];
    decision.cells_are_area=true;
    std::sort(decision.cells.begin(),decision.cells.end(),[](Cell a,Cell b){return a.y==b.y ? a.x<b.x : a.y<b.y;});
    decision.cells.erase(std::unique(decision.cells.begin(),decision.cells.end()),decision.cells.end());
    decision.placed=static_cast<int>(decision.cells.size());
}
