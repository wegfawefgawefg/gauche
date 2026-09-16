#pragma once
#include "../biome.hpp"
#include <array>
#include <cstdint>

// Local generation policy. Only the resulting world enters saves/network state.
// Each biome can register its own themes; slots and compatibility share one roll.
enum class GenerationTheme { None, WetWoods, Timber, Spiders, Mushrooms, Ruins, Overgrowth, Ants, Count };
struct GenerationThemes { GenerationTheme major=GenerationTheme::None,minor=GenerationTheme::None; };
struct ThemeRule {
    GenerationTheme theme;
    const char* name;
    Biome biome;
    std::array<unsigned,4> major,minor;
    std::uint32_t incompatible=0;
};
constexpr std::uint32_t theme_bit(GenerationTheme theme) {return 1U<<static_cast<unsigned>(theme);}
inline constexpr std::array generation_theme_rules{
    ThemeRule{GenerationTheme::None,"None",Biome::Forest,{10,8,5,6},{6,6,5,5}},
    ThemeRule{GenerationTheme::WetWoods,"Wet woods",Biome::Forest,{6,4,4,3},{}},
    ThemeRule{GenerationTheme::Timber,"Timber forest",Biome::Forest,{2,3,4,4},{},theme_bit(GenerationTheme::Overgrowth)},
    ThemeRule{GenerationTheme::Spiders,"Spider territory",Biome::Forest,{0,3,4,3},{}},
    ThemeRule{GenerationTheme::Mushrooms,"Mushroom flush",Biome::Forest,{}, {3,4,4,3}},
    ThemeRule{GenerationTheme::Ruins,"Old ruins",Biome::Forest,{}, {2,3,3,4}},
    ThemeRule{GenerationTheme::Overgrowth,"Undergrowth",Biome::Forest,{}, {3,3,4,3},theme_bit(GenerationTheme::Timber)},
    ThemeRule{GenerationTheme::Ants,"Ant trails",Biome::Forest,{1,2,3,3},{}},
};
static_assert(generation_theme_rules.size()==static_cast<std::size_t>(GenerationTheme::Count));
inline const ThemeRule& theme_rule(GenerationTheme theme) {return generation_theme_rules[static_cast<std::size_t>(theme)];}
inline bool has_theme(GenerationThemes themes,GenerationTheme theme) {return themes.major==theme || themes.minor==theme;}
bool themes_compatible(GenerationTheme a,GenerationTheme b);
unsigned theme_weight(const ThemeRule& rule,int floor,bool minor,GenerationTheme major=GenerationTheme::None);
struct Game;struct FloorPlan;struct GenerationRule;
void select_generation_themes(Game& game,FloorPlan& plan);
unsigned themed_feature_denominator(const GenerationRule& rule,int floor,GenerationThemes themes);
