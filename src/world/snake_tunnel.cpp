#include "snake_tunnel.hpp"
#include "feature_roll.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../items/supply.hpp"
#include "../props/interaction.hpp"
#include "../props/tall_tree.hpp"
#include <algorithm>
#include <array>
#include <cstdlib>

namespace {
Cell scale(Cell c,int n) {return {c.x*n,c.y*n};}
bool eligible(const FloorPlan& plan,int i) {
    return i!=0 && i!=plan.exit_room && i!=plan.objective_room && i!=plan.secret_room &&
        !reserved_habitat(plan.rooms[static_cast<std::size_t>(i)]);
}
void reserve(FloorPlan& plan,Cell c) {
    if (c.x>0 && c.y>0 && c.x<plan.width-1 && c.y<plan.height-1)
        plan.protected_cells[static_cast<std::size_t>(c.y*plan.width+c.x)]=1;
}
void paint(Game& game,FloorPlan& plan,Cell c,TileKind kind) {
    if (auto* t=game.stage.at(c)) {*t={kind};reserve(plan,c);}
}
bool vacant(const Game& game,Cell c) {
    const auto* tile=game.stage.at(c);
    return tile && walkable(*tile) && tile->prop.kind==PropKind::None && entity_at(game,c,false)<0;
}
void shuffle(Game& game,std::vector<Cell>& cells) {
    for (std::size_t i=cells.size();i>1;--i) std::swap(cells[i-1],cells[random_u32(game)%i]);
}
void crossing(Game& game,FloorPlan& plan,SnakeTunnel& tunnel) {
    const Cell last=plan.rooms[static_cast<std::size_t>(tunnel.rooms.back())].center;
    const Cell side{-tunnel.axis.y,tunnel.axis.x};
    const int first_flank=random_u32(game)%2 ? 1 : -1;
    const int desired=1+static_cast<int>(random_u32(game)%3);
    const bool round=random_u32(game)%2!=0;
    // Fit against actual route/gate approaches before reserving the landmark.
    // A smaller moat or the other bank can fit where the first component cannot.
    for (int gap=desired;gap>=1;--gap) for (int flank:{first_flank,-first_flank}) {
        const int extent=gap+3,radius=gap+1;
        const Cell island=last+scale(tunnel.axis,extent)+scale(side,flank*extent);
        bool clear=true;
        for (int y=-radius;y<=radius;++y) for (int x=-radius;x<=radius;++x) {
            if (round && x*x+y*y>radius*radius+radius) continue;
            const Cell c=island+Cell{x,y};
            if (c.x<=0 || c.y<=0 || c.x>=plan.width-1 || c.y>=plan.height-1 || plan.protected_cell(c)) clear=false;
        }
        if (!clear) continue;
        tunnel.island=island;tunnel.crossing_length=gap;tunnel.tree=island-scale(tunnel.axis,gap+2);
        for (int y=-radius;y<=radius;++y) for (int x=-radius;x<=radius;++x) {
            if (round && x*x+y*y>radius*radius+radius) continue;
            paint(game,plan,island+Cell{x,y},std::max(std::abs(x),std::abs(y))<=1 ? TileKind::Grass : TileKind::Chasm);
        }
        for (int i=0;i<=extent;++i) for (int j=-1;j<=1;++j)
            paint(game,plan,last+scale(side,flank*i)+scale(tunnel.axis,j),TileKind::Grass);
        return;
    }
}
bool safe_tree(Game& game,Cell root,Cell dir,int gap) {
    int facing=0;
    constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
    for (int i=0;i<4;++i) if (directions[i]==dir) facing=i;
    if (tree_bridge_length(game,root,dir)!=gap || !vacant(game,root) ||
        !place_prop(game.stage,root,PropKind::TallTree,static_cast<std::uint8_t>(facing))) return false;
    bool valid=floor_reachable(game);
    for (Cell direction:directions) {
        const int length=tree_bridge_length(game,root,direction);if (!length) continue;
        std::array<Tile,3> saved{};
        for (int i=0;i<length;++i) saved[static_cast<std::size_t>(i)]=*game.stage.at(root+scale(direction,i+1));
        lay_tree_bridge(game,root,direction);
        const auto prop=game.stage.at(root)->prop;game.stage.at(root)->prop={};
        valid=valid && floor_lock_required(game);
        game.stage.at(root)->prop=prop;
        for (int i=0;i<length;++i) *game.stage.at(root+scale(direction,i+1))=saved[static_cast<std::size_t>(i)];
    }
    if (!valid) game.stage.at(root)->prop={};
    return valid;
}
}

void plan_snake_tunnel(Game& game,FloorPlan& plan) {
    if (!roll_generation_feature(game,plan,GenerationFeature::SnakeTunnel)) return;
    std::vector<RouteEdge> choices;
    for (auto edge:plan.edges) if (eligible(plan,edge.a) && eligible(plan,edge.b)) choices.push_back(edge);
    if (choices.empty()) { feature_failed(plan,"No adjacent eligible room pair after objective and earlier habitat reservations"); return; }
    plan.report.features.back().candidate_count=static_cast<int>(choices.size());
    auto edge=choices[random_u32(game)%choices.size()];
    if (plan.rooms[static_cast<std::size_t>(edge.a)].depth>plan.rooms[static_cast<std::size_t>(edge.b)].depth) std::swap(edge.a,edge.b);
    SnakeTunnel tunnel;tunnel.rooms={edge.a,edge.b};
    tunnel.axis=plan.rooms[static_cast<std::size_t>(edge.b)].grid-plan.rooms[static_cast<std::size_t>(edge.a)].grid;
    if (random_u32(game)%2==0) for (auto next:plan.edges) {
        const int i=next.a==edge.b ? next.b : next.b==edge.b ? next.a : -1;
        if (i<0 || i==edge.a || !eligible(plan,i)) continue;
        if (plan.rooms[static_cast<std::size_t>(i)].grid-plan.rooms[static_cast<std::size_t>(edge.b)].grid==tunnel.axis) {
            tunnel.rooms.push_back(i);break;
        }
    }
    for (int i:tunnel.rooms) {
        auto& room=plan.rooms[static_cast<std::size_t>(i)];room.shape=RoomShape::SnakeTunnel;room.role=RoomRole::Den;
        room.half_width=7+static_cast<int>(random_u32(game)%3);
        room.half_height=7+static_cast<int>(random_u32(game)%3);
    }
    plan.snake_tunnels.push_back(tunnel);
    feature_reserved(plan,tunnel.rooms,std::to_string(tunnel.rooms.size())+" connected rooms");
}

void carve_snake_tunnel(Game& game,FloorPlan& plan) {
    for (auto& tunnel:plan.snake_tunnels) {
        tunnel.entry=plan.rooms[static_cast<std::size_t>(tunnel.rooms.front())].center;
        tunnel.cache=plan.rooms[static_cast<std::size_t>(tunnel.rooms.back())].center+scale(tunnel.axis,3);
        const Cell side{-tunnel.axis.y,tunnel.axis.x};
        // Replace the former straight neck with a winding solid ribbon across
        // real void. Bend sign, amplitude, width and side ledges roll per link.
        for (std::size_t r=1;r<tunnel.rooms.size();++r) {
            const Cell a=plan.rooms[static_cast<std::size_t>(tunnel.rooms[r-1])].center;
            const Cell b=plan.rooms[static_cast<std::size_t>(tunnel.rooms[r])].center;
            const int n=std::max(std::abs(b.x-a.x),std::abs(b.y-a.y));
            const int amplitude=2+static_cast<int>(random_u32(game)%3),sign=random_u32(game)%2 ? 1 : -1;
            const int width=amplitude+3;const bool ledges=random_u32(game)%2!=0;
            int bend=0;
            for (int i=3;i<=n-3;++i) {
                const Cell c{a.x+(b.x-a.x)*i/n,a.y+(b.y-a.y)*i/n};
                const int progress=(i-3)*amplitude*4/(n-6);
                const int desired=sign*(progress<=amplitude ? progress : progress<=amplitude*3 ? amplitude*2-progress : progress-amplitude*4);
                bend+=std::clamp(desired-bend,-1,1);
                for (int j=-width;j<=width;++j) {
                    const bool dry=i<5 || i>n-5 || std::abs(j-bend)<=1 || (ledges && std::abs(j)==width);
                    paint(game,plan,c+scale(side,j),dry ? TileKind::Grass : TileKind::Chasm);
                }
            }
        }
        // Small bankside holes compose separately from the inter-room chasm.
        for (int index:tunnel.rooms) {
            const auto& room=plan.rooms[static_cast<std::size_t>(index)];
            const int holes=1+static_cast<int>(random_u32(game)%3);
            for (int i=0;i<holes;++i) {
                const Cell c=room.center+Cell{static_cast<int>(random_u32(game)%13)-6,static_cast<int>(random_u32(game)%13)-6};
                if (!plan.protected_cell(c) && walkable(game.stage.at_or_border(c))) paint(game,plan,c,TileKind::Chasm);
            }
        }
        for (int y=-1;y<=1;++y) for (int x=-1;x<=1;++x) paint(game,plan,tunnel.cache+Cell{x,y},TileKind::Grass);
        if (random_u32(game)%3!=0) crossing(game,plan,tunnel);
        for (int index:tunnel.rooms) {
            const auto& room=plan.rooms[static_cast<std::size_t>(index)];
            for (int y=-9;y<=9;++y) for (int x=-9;x<=9;++x) reserve(plan,room.center+Cell{x,y});
        }
    }
}

void populate_snake_tunnel(Game& game,const FloorPlan& plan) {
    for (const auto& tunnel:plan.snake_tunnels) {
        const bool tree=tunnel.crossing_length>0 && safe_tree(game,tunnel.tree,tunnel.axis,tunnel.crossing_length);
        place_ground_item(game,tunnel.entry,ItemKind::HuntingHorn);
        place_ground_item(game,tunnel.cache,ItemKind::ThrowingNet,2);
        place_coins(game,tunnel.cache-tunnel.axis,14+static_cast<int>(random_u32(game)%15));
        if (tree) place_ground_item(game,tunnel.tree-tunnel.axis,ItemKind::Hatchet);
        if (tunnel.crossing_length>0) {
            const auto prize=roll_item_supply(game,LootSource::Weapon,false);
            place_ground_item(game,tunnel.island,prize,supply_count(prize));
            place_coins(game,tunnel.island+tunnel.axis,30+static_cast<int>(random_u32(game)%21));
        }
        std::vector<Cell> banks;
        const Cell first=plan.rooms[static_cast<std::size_t>(tunnel.rooms.front())].center;
        const Cell last=plan.rooms[static_cast<std::size_t>(tunnel.rooms.back())].center;
        for (int y=std::min(first.y,last.y)-9;y<=std::max(first.y,last.y)+9;++y)
            for (int x=std::min(first.x,last.x)-9;x<=std::max(first.x,last.x)+9;++x) {
                const Cell c{x,y};
                if (vacant(game,c) && distance(c,tunnel.entry)>3 && distance(c,tunnel.cache)>2 &&
                    (tunnel.crossing_length==0 || (distance(c,tunnel.island)>5 && distance(c,tunnel.tree)>2))) banks.push_back(c);
            }
        shuffle(game,banks);std::vector<Cell> occupied;
        const int count=static_cast<int>(tunnel.rooms.size())*(3+static_cast<int>(random_u32(game)%3));
        for (Cell c:banks) {
            if (static_cast<int>(occupied.size())>=count) break;
            bool spaced=true;for (Cell old:occupied) if (distance(old,c)<3) spaced=false;
            if (!spaced) continue;
            if (auto* snake=get_entity(game,spawn_entity(game,EntityKind::Snake,c))) {
                snake->timer_a=20+static_cast<int>(random_u32(game)%60);occupied.push_back(c);
            }
        }
        // Low clutter never closes the narrow safe ribbon or disguises a pit.
        for (Cell c:banks) if (vacant(game,c) && random_u32(game)%7==0)
            place_prop(game.stage,c,random_u32(game)%4==0 ? PropKind::BonePile :
                random_u32(game)%2 ? PropKind::Leaves : PropKind::Fern,static_cast<std::uint8_t>(random_u32(game)));
        for (int index:tunnel.rooms) if (game.run.roof_light_count<static_cast<int>(game.run.roof_lights.size()))
            game.run.roof_lights[static_cast<std::size_t>(game.run.roof_light_count++)]=
                {plan.rooms[static_cast<std::size_t>(index)].center,{10,1400,{204,205,149}}};
    }
}
