#include "root_maze.hpp"
#include "feature_roll.hpp"
#include "terrain_material.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../scenery/roof.hpp"
#include "../props/interaction.hpp"
#include "../items/supply.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
Cell scale(Cell c,int n) {return {c.x*n,c.y*n};}
Cell at(const RootMaze& maze,int along,int across) {return maze.origin+scale(maze.axis,along)+scale(maze.side,across);}
bool eligible(const FloorPlan& plan,int i) {
    return i!=0 && i!=plan.exit_room && i!=plan.secret_room && i!=plan.objective_room &&
        !reserved_habitat(plan.rooms[static_cast<std::size_t>(i)]);
}
bool linked(const FloorPlan& plan,int a,int b) {
    for (auto edge:plan.edges) if ((edge.a==a && edge.b==b) || (edge.a==b && edge.b==a)) return true;
    return false;
}
void shuffle(Game& game,std::vector<Cell>& cells) {
    for (std::size_t i=cells.size();i>1;--i) std::swap(cells[i-1],cells[random_u32(game)%i]);
}
void paint(Game& game,const FloorPlan& plan,Cell c,bool root) {
    auto* tile=game.stage.at(c);
    if (!tile || c.x<=0 || c.y<=0 || c.x>=plan.width-1 || c.y>=plan.height-1 || plan.protected_cell(c)) return;
    *tile=root ? wood_tile(TileMaterial::Root) : Tile{TileKind::Grass};
}
void approach(Game& game,const FloorPlan& plan,Cell from,Cell to) {
    while (from!=to) {
        paint(game,plan,from,false);
        if (from.x!=to.x) from.x+=from.x<to.x ? 1 : -1;
        else from.y+=from.y<to.y ? 1 : -1;
    }
    paint(game,plan,to,false);
}
bool vacancy(const Game& game,Cell c) {
    const auto* tile=game.stage.at(c);
    return tile && walkable(*tile) && tile->prop.kind==PropKind::None && entity_at(game,c,false)<0;
}
void hollow_roots(Game& game,const FloorPlan& plan,RootMaze& maze) {
    for (int piece=0;piece<2 && game.stage.roofs.size()<max_roof_spans;++piece) {
        struct Hollow {int start,length;};std::vector<Hollow> choices;
        for (int length=4;length<=7;++length) for (int start=-2;start+length<=maze.length+2;++start) {
            // Leave a solid cut/burn barrier between the two halves of the root.
            if (start-1<=maze.length/2 && start+length>=maze.length/2) continue;
            bool clear=true;
            for (const auto& other:maze.passages)
                if (roof_covers(other,at(maze,start-1,0)) || roof_covers(other,at(maze,start+length,0))) clear=false;
            for (int i=0;i<length;++i) for (int j=-1;j<=1;++j) {
                const auto& tile=game.stage.at_or_border(at(maze,start+i,j));
                if (tile.kind!=TileKind::Wall || tile.material!=TileMaterial::Root) clear=false;
            }
            if (clear) choices.push_back({start,length});
        }
        if (choices.empty()) break;
        const auto chosen=choices[random_u32(game)%choices.size()];
        const Cell first=at(maze,chosen.start,0),last=at(maze,chosen.start+chosen.length-1,0);
        RoofSpan roof;roof.vertical=static_cast<std::uint8_t>(maze.axis.y!=0);
        roof.start={std::min(first.x,last.x)-(roof.vertical ? 1 : 0),std::min(first.y,last.y)-(roof.vertical ? 0 : 1)};
        roof.length=static_cast<std::uint8_t>(chosen.length);
        for (int i=0;i<chosen.length;++i) paint(game,plan,at(maze,chosen.start+i,0),false);
        for (int end:{chosen.start-1,chosen.start+chosen.length})
            for (int j=-3;j<=3;++j) paint(game,plan,at(maze,end,j),false);
        game.stage.roofs.push_back(roof);maze.passages.push_back(roof);
    }
}
}

void plan_root_maze(Game& game,FloorPlan& plan) {
    if (!roll_generation_feature(game,plan,GenerationFeature::RootMaze)) return;
    std::vector<RouteEdge> shortcuts,connected;
    for (int a=1;a<static_cast<int>(plan.rooms.size());++a) for (int b=a+1;b<static_cast<int>(plan.rooms.size());++b) {
        if (!eligible(plan,a) || !eligible(plan,b) || distance(plan.rooms[static_cast<std::size_t>(a)].grid,plan.rooms[static_cast<std::size_t>(b)].grid)!=1) continue;
        (linked(plan,a,b) ? connected : shortcuts).push_back({a,b});
    }
    const auto& choices=shortcuts.empty() ? connected : shortcuts;
    if (choices.empty()) { feature_failed(plan,"No adjacent eligible room pair after objective and earlier habitat reservations"); return; }
    plan.report.features.back().candidate_count=static_cast<int>(choices.size());
    auto pair=choices[random_u32(game)%choices.size()];
    if (plan.rooms[static_cast<std::size_t>(pair.a)].depth>plan.rooms[static_cast<std::size_t>(pair.b)].depth) std::swap(pair.a,pair.b);
    RootMaze maze;maze.a=pair.a;maze.b=pair.b;maze.cross_link=!linked(plan,pair.a,pair.b);
    const auto& a=plan.rooms[static_cast<std::size_t>(pair.a)];const auto& b=plan.rooms[static_cast<std::size_t>(pair.b)];
    maze.axis=b.grid-a.grid;maze.side={-maze.axis.y,maze.axis.x};
    if (random_u32(game)%2) maze.side=scale(maze.side,-1);
    maze.origin=maze.axis.x!=0 ? Cell{a.center.x,(a.center.y+b.center.y)/2} : Cell{(a.center.x+b.center.x)/2,a.center.y};
    maze.origin=maze.origin+scale(maze.side,4+static_cast<int>(random_u32(game)%2));
    maze.length=std::abs((b.center.x-a.center.x)*maze.axis.x+(b.center.y-a.center.y)*maze.axis.y);
    for (int i:{pair.a,pair.b}) {
        auto& room=plan.rooms[static_cast<std::size_t>(i)];room.landmark=true;room.role=RoomRole::Den;
    }
    plan.root_mazes.push_back(maze);
    feature_reserved(plan,std::array{maze.a,maze.b},maze.cross_link ? "New shortcut" : "Existing route branch");
}

void carve_root_maze(Game& game,FloorPlan& plan) {
    for (auto& maze:plan.root_mazes) {
        // Overlay the existing rooms and intervening wall. Their old shapes and
        // route sockets survive; this is an additional branching timber route.
        for (int i=-4;i<=maze.length+4;++i) for (int j=-3;j<=3;++j)
            paint(game,plan,at(maze,i,j),i>=-3 && i<=maze.length+3 && std::abs(j)<=1);
        for (int i=-1;i<maze.length+2;i+=4+static_cast<int>(random_u32(game)%4)) {
            const int sign=random_u32(game)%2 ? 1 : -1;
            const int reach=3+static_cast<int>(random_u32(game)%3);
            for (int j=1;j<=reach;++j) paint(game,plan,at(maze,i,sign*j),true);
            // A fork sometimes creates another pocket, never a fixed twin pattern.
            if (random_u32(game)%3==0) for (int d=1;d<=2;++d) paint(game,plan,at(maze,i+d,sign*(reach-1)),true);
        }
        maze.plug=at(maze,maze.length/2,0);
        for (int j=-3;j<=3;++j) paint(game,plan,at(maze,maze.length/2,j),true);
        maze.entry=at(maze,-4,0);maze.cache=at(maze,maze.length+4,0);
        // End approaches go around the main root rather than deleting its side.
        const Cell a=plan.rooms[static_cast<std::size_t>(maze.a)].center,b=plan.rooms[static_cast<std::size_t>(maze.b)].center;
        const Cell near=a-scale(maze.axis,4),far=b+scale(maze.axis,4);
        approach(game,plan,a,near);approach(game,plan,near,maze.entry);
        approach(game,plan,b,far);approach(game,plan,far,maze.cache);
        hollow_roots(game,plan,maze);
        // Reserve the overlay plus both room footprints from later props/roofs.
        std::vector<Cell> footprint;
        for (int i=-5;i<=maze.length+5;++i) for (int j=-5;j<=5;++j) footprint.push_back(at(maze,i,j));
        for (int index:{maze.a,maze.b}) {
            const auto& room=plan.rooms[static_cast<std::size_t>(index)];
            for (int y=-room.half_height;y<=room.half_height;++y) for (int x=-room.half_width;x<=room.half_width;++x)
                footprint.push_back(room.center+Cell{x,y});
        }
        std::sort(footprint.begin(),footprint.end(),[](Cell a_cell,Cell b_cell){return a_cell.y==b_cell.y ? a_cell.x<b_cell.x : a_cell.y<b_cell.y;});
        footprint.erase(std::unique(footprint.begin(),footprint.end()),footprint.end());
        for (Cell c:footprint) if (game.stage.in_bounds(c)) {
            plan.protected_cells[static_cast<std::size_t>(c.y*plan.width+c.x)]=1;
            const auto& tile=*game.stage.at(c);
            if (tile.kind==TileKind::Wall && tile.material==TileMaterial::Root) maze.roots.push_back(c);
            else if (walkable(tile)) maze.ground.push_back(c);
        }
    }
}

void populate_root_maze(Game& game,const FloorPlan& plan) {
    for (const auto& maze:plan.root_mazes) {
        place_ground_item(game,maze.entry,ItemKind::Hatchet);
        place_ground_item(game,maze.entry-maze.axis,ItemKind::Torch);
        const auto prize=roll_item_supply(game,LootSource::Weapon,false);
        place_ground_item(game,maze.cache,prize,supply_count(prize));
        place_ground_item(game,maze.cache+maze.axis,ItemKind::RootDrill);
        place_coins(game,maze.cache+maze.side,20+static_cast<int>(random_u32(game)%21));
        auto spots=maze.ground;shuffle(game,spots);std::vector<Cell> inhabitants;
        const int count=4+static_cast<int>(random_u32(game)%4);
        for (Cell c:spots) {
            if (static_cast<int>(inhabitants.size())>=count) break;
            if (!vacancy(game,c) || distance(c,maze.entry)<4 || distance(c,maze.cache)<3) continue;
            bool spaced=true;for (Cell old:inhabitants) if (distance(old,c)<4) spaced=false;
            if (!spaced) continue;
            spawn_entity(game,inhabitants.empty() ? EntityKind::RootTurret : random_u32(game)%3==0 ? EntityKind::BrambleGuard : EntityKind::ForagerGoblin,c);
            inhabitants.push_back(c);
        }
        // Small pockets compose around remaining ground, leaving every path open.
        int supplies=1+static_cast<int>(random_u32(game)%3);
        for (Cell c:spots) if (vacancy(game,c) && distance(c,maze.entry)>2 && distance(c,maze.cache)>2) {
            if (supplies>0 && random_u32(game)%9==0) {
                place_ground_item(game,c,random_u32(game)%2 ? ItemKind::Firecracker : ItemKind::FungalBread,2);--supplies;
            } else if (random_u32(game)%4==0)
                place_prop(game.stage,c,random_u32(game)%5==0 ? PropKind::Puffball : random_u32(game)%2 ? PropKind::Leaves : PropKind::Fern,
                    static_cast<std::uint8_t>(random_u32(game)));
        }
        for (Cell c:{maze.entry,maze.cache}) if (game.run.roof_light_count<static_cast<int>(game.run.roof_lights.size()))
            game.run.roof_lights[static_cast<std::size_t>(game.run.roof_light_count++)]={c,{10,1400,{208,191,138}}};
    }
}
