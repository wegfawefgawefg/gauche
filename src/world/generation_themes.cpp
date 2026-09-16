#include "generation_themes.hpp"
#include "route.hpp"
#include "components.hpp"
#include <algorithm>

bool themes_compatible(GenerationTheme a,GenerationTheme b) {
    return a==GenerationTheme::None || b==GenerationTheme::None ||
        (a!=b && !(theme_rule(a).incompatible&theme_bit(b)) && !(theme_rule(b).incompatible&theme_bit(a)));
}
unsigned theme_weight(const ThemeRule& rule,int floor,bool minor,GenerationTheme major) {
    if (!biome_floor(floor,rule.biome) || (minor && !themes_compatible(major,rule.theme))) return 0;
    return (minor ? rule.minor : rule.major)[static_cast<std::size_t>(biome_stage(floor)-1)];
}
void select_generation_themes(Game& game,FloorPlan& plan) {
    FeatureDecision decision;decision.feature=GenerationFeature::Themes;
    decision.denominator=feature_denominator(generation_rule(decision.feature),game.run.floor);
    if (!decision.denominator) {
        decision.outcome=GenerationOutcome::Ineligible;decision.reason="No theme rules registered for this biome yet";
        plan.report.features.push_back(std::move(decision));return;
    }
    const auto select=[&](bool minor,int parent) {
        std::vector<WeightedComponent> choices;
        for (const auto& rule:generation_theme_rules)
            choices.push_back({static_cast<int>(rule.theme),rule.name,theme_weight(rule,game.run.floor,minor,plan.themes.major)});
        return roll_component(game,&plan.report,GenerationFeature::Themes,parent,minor ? "Minor modifier" : "Major identity",{},choices);
    };
    const auto major=select(false,-1);plan.themes.major=static_cast<GenerationTheme>(major.value);plan.theme_component=major.record;
    const auto minor=select(true,major.record);plan.themes.minor=static_cast<GenerationTheme>(minor.value);
    plan.minor_theme_component=minor.record;
    component_result(&plan.report,major,major.value==0 ? "No dominant identity; ordinary feature rolls remain" : "Policy selected; room roles, terrain, features and inhabitants follow");
    component_result(&plan.report,minor,plan.themes.major==GenerationTheme::Timber ?
        "Undergrowth excluded as redundant with timber; remaining weights renormalized" : "Compatible stage-weighted modifier selected (None leaves ordinary scenery)");
    plan.report.themes=plan.themes;
    decision.outcome=GenerationOutcome::Selected;
    decision.reason="One major identity and one compatible minor modifier; None is a real outcome in each slot. Weights are conditional, not independent floor percentages.";
    decision.variant=std::string(theme_rule(plan.themes.major).name)+" + "+theme_rule(plan.themes.minor).name;
    plan.report.features.push_back(std::move(decision));
}
unsigned themed_feature_denominator(const GenerationRule& rule,int floor,GenerationThemes themes) {
    unsigned value=feature_denominator(rule,floor);
    if (!value || rule.biome!=Biome::Forest) return value;
    const auto boost=[&](unsigned limit){value=std::min(value,limit);};
    if (has_theme(themes,GenerationTheme::WetWoods)) {
        if (rule.feature==GenerationFeature::River) boost(1);
        if (rule.feature==GenerationFeature::OpenSectors) boost(2);
    }
    if (has_theme(themes,GenerationTheme::Timber)) {
        if (rule.feature==GenerationFeature::TimberGrove) boost(2);
        if (rule.feature==GenerationFeature::GiantTree) boost(3);
        if (rule.feature==GenerationFeature::OpenSectors) boost(2);
    }
    if (has_theme(themes,GenerationTheme::Spiders) && rule.feature==GenerationFeature::SpiderCave) boost(1);
    if (has_theme(themes,GenerationTheme::Overgrowth)) {
        if (rule.feature==GenerationFeature::GiantTree || rule.feature==GenerationFeature::RootMaze) boost(3);
    }
    if (has_theme(themes,GenerationTheme::Ants) && rule.feature==GenerationFeature::AntColonies) boost(1);
    if (rule.feature==GenerationFeature::MushroomSettlements) {
        if (has_theme(themes,GenerationTheme::GnomeWoods)) boost(1);
        else if (has_theme(themes,GenerationTheme::Mushrooms)) boost(2);
    }
    return value;
}
