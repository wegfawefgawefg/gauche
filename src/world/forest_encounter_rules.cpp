#include "forest_encounter_rules.hpp"

namespace {
using Options=std::vector<WeightedComponent>;
void option(Options& pool,EntityKind kind,const char* name,unsigned weight) {
    pool.push_back({static_cast<int>(kind),name,weight});
}
}

ForestEncounterRules forest_encounter_rules(int floor,RoomRole role,GenerationThemes themes) {
    ForestEncounterRules rules;
    if(!forest_floor(floor)){rules.exclusion="Outside Forest; this ordinary encounter policy does not apply";return rules;}
    if(role==RoomRole::Entrance || role==RoomRole::Exit || role==RoomRole::Secret) {
        rules.exclusion="Entrance, exit and secret rooms do not receive ordinary fighter packs";return rules;
    }
    if(role>RoomRole::Secret){rules.exclusion="Room role belongs to another biome";return rules;}
    const int stage=biome_stage(floor);
    const bool wet=role==RoomRole::Brook || has_theme(themes,GenerationTheme::WetWoods);
    const bool ruins=role==RoomRole::Ruins || has_theme(themes,GenerationTheme::Ruins);
    const bool hunters=stage>=2,spiders=has_theme(themes,GenerationTheme::Spiders);
    rules.spacing={{0,"Loose inhabitants",3},{1,"Close pack",4},{2,"Two pockets",stage>=2 ? 3U : 1U}};
    rules.sizes={{2,"Pair",stage==1 ? 5U : 2U},{3,"Three",4},{4,"Four",stage==1 ? 1U : 4U},{5,"Five",stage>=3 ? 3U : 0U},{6,"Six",stage>=3 ? 1U : 0U}};
    rules.families={{0,"Winged pests",wet ? 6U : 3U},{1,"Restless dead",ruins ? 7U : 2U},
        {2,"Hunting animals",hunters ? 4U : 0U},{3,"Mixed prowlers",hunters ? 3U : 0U},{4,"Spider hunters",spiders ? 9U : 0U}};
    if(!hunters)rules.family_exclusions[2]=rules.family_exclusions[3]="Begins on Forest 1-2";
    if(!spiders)rules.family_exclusions[4]="Requires the Spider territory floor identity";
    option(rules.members[0],EntityKind::Mosquito,"Mosquito",wet ? 5U : 2U);option(rules.members[0],EntityKind::Bat,"Bat",wet ? 2U : 5U);
    option(rules.members[1],EntityKind::Zombie,"Zombie",8);option(rules.members[1],EntityKind::Bat,"Bat",2);
    option(rules.members[2],EntityKind::Wolf,"Wolf",5);option(rules.members[2],EntityKind::Boar,"Boar",2);
    option(rules.members[3],EntityKind::Zombie,"Zombie",3);option(rules.members[3],EntityKind::Wolf,"Wolf",3);
    option(rules.members[3],EntityKind::Bat,"Bat",2);option(rules.members[3],EntityKind::Mosquito,"Mosquito",2);
    option(rules.members[4],EntityKind::ForestSpider,"Forest spider",8);option(rules.members[4],EntityKind::Bat,"Bat",2);

    auto& choices=rules.specialists;
    const unsigned later=stage>=2 ? 1U : 0U;
    option(choices,EntityKind::None,"No specialist",stage>=3 ? 3U : 6U);
    rules.specialist_note=stage==1 ? "Ordinary specialists start on Forest 1-2. Landmark inhabitants are independent." :
        role==RoomRole::Ruins ? "Stacks and undead sources enter the ruins pool on Forest 1-3." :
        role==RoomRole::Thicket || role==RoomRole::Den ? "Burrow worms enter this room's pool on Forest 1-3." :
        "This additional pool follows the room role; its choices do not replace ordinary fighters.";
    // Keep zero-weight candidates visible on earlier stages. They consume no
    // ticket space and cannot change which nonzero candidate a roll selects.
    switch(role) {
    case RoomRole::Thicket:
        option(choices,EntityKind::RootTurret,"Root watch",3*later);
        option(choices,EntityKind::ThornSnail,"Thorn snail",2*later);
        option(choices,EntityKind::WaspNest,"Wasp nest",2*later);
        option(choices,EntityKind::BurrowWorm,"Burrow worm",stage>=3 ? 2U : 0U);break;
    case RoomRole::Den:
        option(choices,EntityKind::Den,"Wolf den",3*later);
        option(choices,EntityKind::BurrowWorm,"Burrow worm",stage>=3 ? 2U : 0U);break;
    case RoomRole::Ruins:
        option(choices,EntityKind::ZombieStack,"Zombie stack",stage>=3 ? 4U : 0U);
        option(choices,EntityKind::Spawner,"Undead source",stage>=3 ? 2U : 0U);break;
    case RoomRole::Cache:case RoomRole::Workshop:option(choices,EntityKind::CrateMimic,"Mimic",3*later);break;
    case RoomRole::Shrine:
        option(choices,EntityKind::RootTurret,"Root watch",2*later);
        option(choices,EntityKind::Bear,"Lone bear",later);break;
    case RoomRole::Orchard:option(choices,EntityKind::WaspNest,"Wasp nest",3*later);break;
    case RoomRole::Brook:option(choices,EntityKind::SporeToad,"Spore toad",3*later);break;
    case RoomRole::Clearing:
        option(choices,EntityKind::Owl,"Owl perch",2*later);
        option(choices,EntityKind::ThornSnail,"Thorn snail",2*later);
        option(choices,EntityKind::RootTurret,"Root watch",2*later);break;
    default:option(choices,EntityKind::Owl,"Owl perch",2*later);break;
    }
    auto& wildlife=rules.wildlife;
    option(wildlife,EntityKind::None,"No wildlife",3);
    option(wildlife,EntityKind::LanternMoth,"Lantern moth",2);
    option(wildlife,EntityKind::CarrionCrow,"Scavenging crow",role==RoomRole::Ruins || role==RoomRole::Den ? 3U : 1U);
    if(role==RoomRole::Brook)option(wildlife,EntityKind::Bunny,"Rabbit",4);
    if(role==RoomRole::Orchard || role==RoomRole::Clearing)option(wildlife,EntityKind::Chicken,"Chicken family",4);
    if(role==RoomRole::Workshop || role==RoomRole::Cache) {
        option(wildlife,EntityKind::ForagerGoblin,"Forager",3);option(wildlife,EntityKind::Dog,"Dog",1);
        option(wildlife,EntityKind::Woodpecker,"Woodpecker",2);
    }
    return rules;
}
