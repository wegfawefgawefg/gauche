#include "spider_cave.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../items/supply.hpp"
#include "../entities/forest_spider.hpp"
#include "../props/interaction.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
bool eligible(const FloorPlan& plan,int i) {
    return i!=0 && i!=plan.exit_room && i!=plan.objective_room && i!=plan.secret_room &&
        !reserved_habitat(plan.rooms[static_cast<std::size_t>(i)]);
}
void reserve(FloorPlan& plan,Cell c) {
    if (c.x>=0 && c.y>=0 && c.x<plan.width && c.y<plan.height)
        plan.protected_cells[static_cast<std::size_t>(c.y*plan.width+c.x)]=1;
}
void open(Game& game,FloorPlan& plan,Cell c,bool grass=false) {
    if (auto* t=game.stage.at(c)) {*t={grass ? TileKind::Grass : TileKind::Empty};reserve(plan,c);}
}
bool vacant(const Game& game,Cell c) {
    const auto* t=game.stage.at(c);
    return t && walkable(*t) && t->prop.kind==PropKind::None && entity_at(game,c,false)<0;
}
void shuffle(Game& game,std::vector<Cell>& cells) {
    for (std::size_t i=cells.size();i>1;--i) std::swap(cells[i-1],cells[random_u32(game)%i]);
}
}

void plan_spider_cave(Game& game,FloorPlan& plan) {
    if (!forest_floor(game.run.floor) || random_u32(game)%(game.run.floor==1 ? 5U : 3U)!=0) return;
    std::vector<RouteEdge> choices;
    for (auto edge:plan.edges) if (eligible(plan,edge.a) && eligible(plan,edge.b)) choices.push_back(edge);
    if (choices.empty()) return;
    auto edge=choices[random_u32(game)%choices.size()];
    if (plan.rooms[static_cast<std::size_t>(edge.a)].depth>plan.rooms[static_cast<std::size_t>(edge.b)].depth) std::swap(edge.a,edge.b);
    SpiderCave cave;cave.rooms={edge.a,edge.b};
    const Cell direction=plan.rooms[static_cast<std::size_t>(edge.b)].grid-plan.rooms[static_cast<std::size_t>(edge.a)].grid;
    if (random_u32(game)%2==0) for (auto next:plan.edges) {
        const int index=next.a==edge.b ? next.b : next.b==edge.b ? next.a : -1;
        if (index<0 || index==edge.a || !eligible(plan,index)) continue;
        if (plan.rooms[static_cast<std::size_t>(index)].grid-plan.rooms[static_cast<std::size_t>(edge.b)].grid==direction) {
            cave.rooms.push_back(index);break;
        }
    }
    cave.mother=game.run.floor>1 && random_u32(game)%3!=0;
    for (int i:cave.rooms) {
        auto& room=plan.rooms[static_cast<std::size_t>(i)];room.shape=RoomShape::SpiderCave;room.role=RoomRole::Den;
        room.half_width=7+static_cast<int>(random_u32(game)%3);room.half_height=7+static_cast<int>(random_u32(game)%3);
    }
    plan.spider_caves.push_back(cave);
}

void carve_spider_cave(Game& game,FloorPlan& plan) {
    for (auto& cave:plan.spider_caves) {
        const auto& first=plan.rooms[static_cast<std::size_t>(cave.rooms.front())];
        const auto& last=plan.rooms[static_cast<std::size_t>(cave.rooms.back())];
        const Cell axis=cardinal_toward(first.center,last.center,{1,0}),side{-axis.y,axis.x};
        cave.entry=first.center;
        // Inter-room necks widen and narrow; preserve all pre-existing route sockets.
        for (std::size_t r=1;r<cave.rooms.size();++r) {
            const Cell a=plan.rooms[static_cast<std::size_t>(cave.rooms[r-1])].center,b=plan.rooms[static_cast<std::size_t>(cave.rooms[r])].center;
            const int n=std::max(std::abs(b.x-a.x),std::abs(b.y-a.y));
            const int bulge=2+static_cast<int>(random_u32(game)%3);
            for (int i=0;i<=n;++i) {
                const Cell c{a.x+(b.x-a.x)*i/n,a.y+(b.y-a.y)*i/n};
                const int radius=(i>n/3 && i<n*2/3) ? bulge : 2;
                for (int j=-radius;j<=radius;++j) open(game,plan,c+Cell{side.x*j,side.y*j},std::abs(j)>1);
            }
        }
        const int flank=random_u32(game)%2 ? 1 : -1;
        cave.cache=last.center+Cell{side.x*flank*4,side.y*flank*4};
        for (int y=-2;y<=2;++y) for (int x=-2;x<=2;++x)
            if (x*x+y*y<=5) open(game,plan,cave.cache+Cell{x,y},true);
        // Room-local rolls compose several uneven web banks, rather than stamping
        // a complete identical web room. The center and route sockets stay dry.
        for (int index:cave.rooms) {
            const auto& room=plan.rooms[static_cast<std::size_t>(index)];
            std::vector<Cell> sites;
            for (int y=-room.half_height;y<=room.half_height;++y)
                for (int x=-room.half_width;x<=room.half_width;++x) {
                    const Cell c=room.center+Cell{x,y};
                    if (walkable(game.stage.at_or_border(c)) && distance(c,cave.entry)>3 && distance(c,cave.cache)>1)
                        sites.push_back(c);
                    reserve(plan,c);
                }
            shuffle(game,sites);
            const int groups=3+static_cast<int>(random_u32(game)%4);
            for (int group=0;group<groups && !sites.empty();++group) {
                const Cell anchor=sites.back();sites.pop_back();
                const int radius=1+static_cast<int>(random_u32(game)%3);
                for (Cell c:sites) if (distance(c,anchor)<=radius && random_u32(game)%4!=0 &&
                    std::find(cave.webs.begin(),cave.webs.end(),c)==cave.webs.end()) cave.webs.push_back(c);
                cave.broods.push_back(anchor);
            }
        }
    }
}

void populate_spider_cave(Game& game,const FloorPlan& plan) {
    for (const auto& cave:plan.spider_caves) {
        for (Cell c:cave.webs) if (vacant(game,c))
            place_prop(game.stage,c,PropKind::ForestWeb,static_cast<std::uint8_t>(random_u32(game)%3));
        // Reserve all adults first; one brood's remains or young cannot occupy
        // the next brood's anchor before its adult is installed.
        for (Cell center:cave.broods) {
            if (!walkable(game.stage.at_or_border(center)) || entity_at(game,center,false)>=0) continue;
            const auto adult=spawn_entity(game,EntityKind::ForestSpider,center);
            if (auto* spider=get_entity(game,adult)) {
                set_forest_spider_role(*spider,SpiderAdult);
                // Stagger waking/recovery; a brood should not bite in perfect sync.
                spider->timer_a=30+static_cast<int>(random_u32(game)%61);
            }
        }
        for (Cell center:cave.broods) {
            const int young=random_u32(game)%3==0 ? 1+static_cast<int>(random_u32(game)%3) : 0;
            for (int j=0;j<young;++j) {
                const Cell c=center+Cell{static_cast<int>(random_u32(game)%5)-2,static_cast<int>(random_u32(game)%5)-2};
                const auto* tile=game.stage.at(c);
                if (!tile || !walkable(*tile) || entity_at(game,c,false)>=0 || distance(c,cave.cache)<=2) continue;
                if (auto* spider=get_entity(game,spawn_entity(game,EntityKind::ForestSpider,c))) set_forest_spider_role(*spider,SpiderYoung);
            }
            for (Cell d:{Cell{1,0},Cell{-1,0},Cell{0,1},Cell{0,-1}}) {
                const Cell c=center+d;
                if (vacant(game,c) && distance(c,cave.cache)>2 && random_u32(game)%3==0)
                    place_prop(game.stage,c,random_u32(game)%2 ? PropKind::BonePile : PropKind::RottenLog);
            }
        }
        if (cave.mother) {
            const Cell c=cave.cache+Cell{0,1};
            if (auto* spider=get_entity(game,spawn_entity(game,EntityKind::ForestSpider,c))) set_forest_spider_role(*spider,SpiderMother);
        }
        // Clear, finite native equipment rewards; web removal also works with fists.
        const auto weapon=roll_item_supply(game,LootSource::Weapon,false);
        place_ground_item(game,cave.cache,weapon,supply_count(weapon));
        const bool rope=random_u32(game)%2!=0;
        place_ground_item(game,cave.cache+Cell{1,0},rope ? ItemKind::RopeHook : ItemKind::ThrowingNet,rope ? 1 : 2);
        place_coins(game,cave.cache+Cell{-1,0},18+static_cast<int>(random_u32(game)%19));
        place_ground_item(game,cave.entry,ItemKind::Torch);
        if (game.run.roof_light_count<static_cast<int>(game.run.roof_lights.size()))
            game.run.roof_lights[static_cast<std::size_t>(game.run.roof_light_count++)]={cave.entry,{7,1100,{194,196,158}}};
    }
}
