#include "world/population_report.hpp"
#include "world/generation_report.hpp"
#include <cstdio>
#include <memory>
#define FOREST_REPORTS 1
int threat(const Entity& actor) {
    if(actor.health<=0)return 0;
    switch(actor.kind) {
    case EntityKind::Zombie:case EntityKind::ZombieStack:case EntityKind::Bat:
    case EntityKind::Wolf:case EntityKind::Boar:case EntityKind::Mosquito:
    case EntityKind::Snake:case EntityKind::ForestSpider:case EntityKind::Wasp:return 1;
    case EntityKind::BurrowWorm:return actor.label_a==0 ? 1 : 0;
    case EntityKind::RootTurret:case EntityKind::BrambleGuard:case EntityKind::ThornSnail:
    case EntityKind::SporeToad:case EntityKind::Owl:return 2;
    case EntityKind::Bear:case EntityKind::CrateMimic:return 3;
    case EntityKind::Chicken:case EntityKind::Bunny:case EntityKind::Dog:
    case EntityKind::LanternMoth:case EntityKind::Woodpecker:case EntityKind::ForagerGoblin:
    case EntityKind::CarrionCrow:return 5;
    case EntityKind::Den:case EntityKind::Spawner:case EntityKind::WaspNest:
    case EntityKind::Encounter:case EntityKind::WaveVent:return 6;
#ifdef FOREST_REPORTS
    case EntityKind::Ant:case EntityKind::Gnome:return 3;
    case EntityKind::OldGrowthBear:return 4;
#endif
    default:return 0;
    }
}

int main() {
    auto g=std::make_unique<Game>();
    std::puts("floor,seed,mobile,specialist,reactive,boss,passive,sources,toads,bears,wolves,bats,zombies,spiders,ants,gnomes,giant_tree,timber,snake,spider_cave,bear_den,root_maze,river,open_sectors,ant_colony,mushroom_district,major,minor");
    for(int floor=1;floor<=4;++floor)for(unsigned seed=1;seed<=256;++seed) {
        *g={};g->run.floor=floor;g->rng=g->run.seed=seed;player_state(*g,0).online=true;
        PopulationReport p;generate_world_floor(*g,FloorLayout::Generated,&p);
        int counts[7]{};int animals[8]{};
        constexpr EntityKind species[]{EntityKind::SporeToad,EntityKind::Bear,EntityKind::Wolf,EntityKind::Bat,EntityKind::Zombie,EntityKind::ForestSpider,EntityKind::Ant,EntityKind::Gnome};
        for(const auto& e:g->entities){++counts[threat(e)];for(int j=0;j<8;++j)animals[j]+=e.kind==species[j] && e.health>0;}
        std::printf("%d,%u",floor,seed);
        for(int j=1;j<=6;++j)std::printf(",%d",counts[j]);
        for(int n:animals)std::printf(",%d",n);
        std::printf(",%zu,%zu,%zu,%zu,%zu,%zu",p.giant_trees.size(),p.timber_groves.size(),p.snake_tunnels.size(),p.spider_caves.size(),p.forest_dens.size(),p.root_mazes.size());
        for(auto f:{GenerationFeature::River,GenerationFeature::OpenSectors,GenerationFeature::AntColonies,GenerationFeature::MushroomSettlements}) {
            const auto* d=feature_decision(*g->generation_report,f);std::printf(",%d",d && d->outcome==GenerationOutcome::Built);
        }
        std::printf(",%d,%d\n",static_cast<int>(g->generation_report->themes.major),static_cast<int>(g->generation_report->themes.minor));
    }
}
