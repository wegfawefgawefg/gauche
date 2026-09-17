#include "generation_audit.hpp"
#include "../world/population_report.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <span>
#include <vector>
#if __has_include("../world/generation_report.hpp")
#include "../world/generation_report.hpp"
#define FOREST_REPORTS 1
#endif

namespace {
// Count heads, not worm segments. Territorial specialists are separate from
// ordinary mobile fighters; sleeping/conditional defenders never inflate either.
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
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
std::vector<int> walking_distances(const Game& game,std::span<const Cell> starts) {
    std::vector<int> distances(game.stage.tiles.size(),-1);std::vector<Cell> queue;
    for(Cell cell:starts)if(game.stage.in_bounds(cell)) {
        const auto i=static_cast<std::size_t>(cell.y*game.stage.width+cell.x);
        if(distances[i]<0){distances[i]=0;queue.push_back(cell);}
    }
    for(std::size_t n=0;n<queue.size();++n)for(Cell d:sides) {
        const Cell cell=queue[n]+d;const auto* tile=game.stage.at(cell);
        if(!tile || !walkable(*tile))continue;
        const auto i=static_cast<std::size_t>(cell.y*game.stage.width+cell.x);
        if(distances[i]>=0)continue;
        distances[i]=distances[static_cast<std::size_t>(queue[n].y*game.stage.width+queue[n].x)]+1;queue.push_back(cell);
    }
    return distances;
}
int quiet_component(const Game& game,std::span<const std::uint8_t> quiet) {
    std::vector<bool> seen(quiet.size());int largest=0;
    for(int y=0;y<game.stage.height;++y)for(int x=0;x<game.stage.width;++x) {
        const auto index=static_cast<std::size_t>(y*game.stage.width+x);
        if(!quiet[index] || seen[index])continue;
        std::vector<Cell> queue{{x,y}};seen[index]=true;
        for(std::size_t n=0;n<queue.size();++n)for(Cell d:sides) {
            const Cell cell=queue[n]+d;if(!game.stage.in_bounds(cell))continue;
            const auto i=static_cast<std::size_t>(cell.y*game.stage.width+cell.x);
            if(!seen[i] && quiet[i]){seen[i]=true;queue.push_back(cell);}
        }
        largest=std::max(largest,static_cast<int>(queue.size()));
    }
    return largest;
}
}

// Ordinary generation only, independent fixed seeds on each stage, no simulation.
// Distance coverage is potential contact, not LOS, aggro range or a difficulty score.
// Required doors are considered open; living actors do not block this measurement.
int run_forest_audit() {
    auto game=std::make_unique<Game>();
    std::puts("floor,seed,reachable,walkable,mobile_fighters,specialists,reactive,bosses,near_fighter,quiet_largest,ordinary_rooms,empty_rooms,props,interactive_props,loose_items,giant_tree,timber_grove,snake_pit,spider_cave,bear_den,root_maze,major,minor,open_sectors,rivers,ant_colonies,mushroom_districts,component_rows,report_capped,passive_life,enemy_sources,generation_us");
    for(int floor=1;floor<=4;++floor)for(unsigned seed=1;seed<=16;++seed) {
        *game={};game->run.floor=floor;game->rng=game->run.seed=seed;player_state(*game, 0).online=true;
        PopulationReport report;
        const auto start=std::chrono::steady_clock::now();
        generate_world_floor(*game,FloorLayout::Generated,&report);
        const auto generation_us=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count();
        if(!floor_reachable(*game) || !floor_lock_required(*game)){std::fprintf(stderr,"Invalid route: floor%d seed%u\n",floor,seed);return 1;}
        std::array<int,7> counts{};int items=0;std::vector<Cell> fighters;
        for(const auto& actor:game->entities) {
            const int category=threat(actor);if(category)++counts[static_cast<std::size_t>(category)];
            if(category==1 || category==2 || category==4)fighters.push_back(actor.cell);
            items+=actor.kind==EntityKind::GroundItem;
        }
        const auto reachable=walking_distances(*game,std::array{game->run.spawn});
        const auto nearest=walking_distances(*game,fighters);
        int area=0,walk=0,near=0,props=0,interactive=0;
        std::vector<std::uint8_t> quiet(game->stage.tiles.size());
        for(int y=0;y<game->stage.height;++y)for(int x=0;x<game->stage.width;++x) {
            const Cell cell{x,y};const auto i=static_cast<std::size_t>(y*game->stage.width+x);const auto& tile=game->stage.tiles[i];
            if(walkable(tile)) {
                ++walk;
                if(reachable[i]>=0){++area;if(nearest[i]>=0 && nearest[i]<=8)++near;else quiet[i]=1;}
            }
            bool accessible=reachable[i]>=0;
            for(Cell d:sides)if(game->stage.in_bounds(cell+d))accessible|=reachable[static_cast<std::size_t>((y+d.y)*game->stage.width+x+d.x)]>=0;
            if(accessible && tile.prop.kind!=PropKind::None && !tile.prop.broken) {
                ++props;interactive+=tile.prop.kind!=PropKind::Leaves && tile.prop.kind!=PropKind::Twigs && tile.prop.kind!=PropKind::Fern && tile.prop.kind!=PropKind::TallGrass;
            }
        }
        int rooms=0,empty=0;
        for(const auto& room:report.rooms) {
            if(reserved_habitat(room) || room.role==RoomRole::Entrance || room.role==RoomRole::Exit || room.role==RoomRole::Shrine || room.role==RoomRole::Secret)continue;
            ++rooms;bool populated=false;
            for(Cell cell:fighters)if(std::abs(cell.x-room.center.x)<=room.half_width && std::abs(cell.y-room.center.y)<=room.half_height)populated=true;
            empty+=!populated;
        }
        int major=0,minor=0,sectors=0,rivers=0,ants=0,mushrooms=0,rows=0,capped=0;
#ifdef FOREST_REPORTS
        const auto& diagnostics=*game->generation_report;
        major=static_cast<int>(diagnostics.themes.major);minor=static_cast<int>(diagnostics.themes.minor);
        for(const auto& decision:diagnostics.features)if(decision.outcome==GenerationOutcome::Built) {
            sectors+=decision.feature==GenerationFeature::OpenSectors;rivers+=decision.feature==GenerationFeature::River;
            ants+=decision.feature==GenerationFeature::AntColonies;mushrooms+=decision.feature==GenerationFeature::MushroomSettlements;
        }
        rows=static_cast<int>(diagnostics.components.size());capped=diagnostics.components_truncated;
#endif
        std::printf("%d,%u,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%zu,%zu,%zu,%zu,%zu,%zu,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%lld\n",floor,seed,area,walk,counts[1],counts[2],counts[3],counts[4],near,quiet_component(*game,quiet),rooms,empty,props,interactive,items,report.giant_trees.size(),report.timber_groves.size(),report.snake_tunnels.size(),report.spider_caves.size(),report.forest_dens.size(),report.root_mazes.size(),major,minor,sectors,rivers,ants,mushrooms,rows,capped,counts[5],counts[6],static_cast<long long>(generation_us));
    }
    return 0;
}
