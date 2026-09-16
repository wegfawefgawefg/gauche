#include "giant_tree.hpp"
#include "landmark_supplies.hpp"
#include "feature_roll.hpp"
#include "four_room_block.hpp"
#include "terrain_material.hpp"
#include "../scenery/hollow_tree.hpp"
#include "../entities/forest_spider.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

namespace {
constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
Cell scale(Cell c,int n) {return {c.x*n,c.y*n};}
void shuffle(Game& game,std::vector<Cell>& cells) {
    for (std::size_t i=cells.size();i>1;--i) std::swap(cells[i-1],cells[random_u32(game)%i]);
}
void floor(Game& game,Cell c) {if (auto* t=game.stage.at(c)) *t={TileKind::Grass};}
void root(Game& game,const FloorPlan& plan,Cell c) {
    if (!plan.protected_cell(c)) if (auto* t=game.stage.at(c)) *t=wood_tile(TileMaterial::Root);
}
bool vacant(const Game& game,Cell c) {
    const auto* t=game.stage.at(c);return t && walkable(*t) && t->prop.kind==PropKind::None && entity_at(game,c,false)<0;
}
void entrance(Game& game,const FloorPlan& plan,GiantTree& tree,Cell dir,int offset) {
    const Cell center=tree.canopy.start+Cell{tree.canopy.length/2,tree.canopy.width/2};
    const Cell side{-dir.y,dir.x};
    const int radius=dir.x ? tree.canopy.length/2 : tree.canopy.width/2;
    const Cell a=center+scale(dir,radius-4)+scale(side,offset),b=a+scale(dir,6);
    int supports=0;
    for (int i=0;i<7;++i) for (int j=-1;j<=1;++j) {
        const Cell c=a+scale(dir,i)+scale(side,j);
        if (j==0) floor(game,c);else root(game,plan,c);
        const auto& t=game.stage.at_or_border(c);
        supports+=j!=0 && t.kind==TileKind::Wall && wooden_terrain(t);
    }
    floor(game,a-dir);floor(game,b+dir);tree.entrances.push_back(b+dir);
    if (supports<7 || game.stage.roofs.size()>=max_roof_spans-1) return;
    RoofSpan passage;passage.vertical=static_cast<std::uint8_t>(dir.y!=0);passage.length=7;
    passage.start={std::min(a.x,b.x)-(dir.y ? 1 : 0),std::min(a.y,b.y)-(dir.x ? 1 : 0)};
    game.stage.roofs.push_back(passage);tree.passages.push_back(passage);
}
void inner_roots(Game& game,const FloorPlan& plan,const GiantTree& tree,Cell center) {
    const int count=4+static_cast<int>(random_u32(game)%5);
    for (int i=0;i<count;++i) {
        Cell tip=center+Cell{static_cast<int>(random_u32(game)%23)-11,static_cast<int>(random_u32(game)%23)-11};
        Cell direction=directions[random_u32(game)%4];
        const int length=2+static_cast<int>(random_u32(game)%5);
        for (int step=0;step<length;++step) {
            if (!tree_ellipse(tree.canopy,tip,5) || distance(tip,tree.cache)<4 || distance(tip,center)<3) break;
            root(game,plan,tip);
            const Cell side{-direction.y,direction.x};
            if (step>0 && random_u32(game)%3==0) {
                const Cell fork=tip+side;
                if (tree_ellipse(tree.canopy,fork,5) && distance(fork,tree.cache)>=4) root(game,plan,fork);
            }
            if (random_u32(game)%4==0) direction=random_u32(game)%2 ? side : scale(side,-1);
            tip=tip+direction;
        }
    }
}
}

void plan_giant_tree(Game& game,FloorPlan& plan) {
    if (!roll_generation_feature(game,plan,GenerationFeature::GiantTree)) return;
    GiantTree tree;
    const auto chosen=reserve_four_rooms(game,plan,tree.rooms,&plan.report.features.back().candidate_count);
    if (!chosen) { feature_failed(plan,"No eligible four-room block; objectives and earlier habitats are excluded"); return; }
    const Cell center=*chosen;
    tree.canopy.kind=RoofKind::HollowTree;tree.canopy.hp=160;
    tree.canopy.length=static_cast<std::uint8_t>(33+2*(random_u32(game)%3));
    tree.canopy.width=static_cast<std::uint8_t>(33+2*(random_u32(game)%3));
    tree.canopy.height=static_cast<std::uint8_t>(4+random_u32(game)%3);
    tree.canopy.start=center-Cell{tree.canopy.length/2,tree.canopy.width/2};
    tree.cache=center+scale(directions[random_u32(game)%4],4+static_cast<int>(random_u32(game)%3));
    tree.spiders=random_u32(game)%3==0;
    plan.giant_trees.push_back(tree);
    feature_reserved(plan,tree.rooms,tree.spiders ? "Spider hollow" : "Forager hollow");
}

void carve_giant_tree(Game& game,FloorPlan& plan) {
    for (auto& tree:plan.giant_trees) {
        const auto& roof=tree.canopy;
        const Cell center=roof.start+Cell{roof.length/2,roof.width/2};
        // One hollow interior spans the former room boundaries. Existing external
        // socket paths stay open through its shell, alongside rolled root mouths.
        for (int y=0;y<roof.width;++y) for (int x=0;x<roof.length;++x) {
            const Cell c=roof.start+Cell{x,y};
            if (tree_ellipse(roof,c,3)) floor(game,c);
            else if (tree_ellipse(roof,c)) root(game,plan,c);
        }
        for (Cell dir:directions) {
            const int radius=dir.x ? roof.length/2 : roof.width/2;
            const Cell side{-dir.y,dir.x};const int bend=static_cast<int>(random_u32(game)%5)-2;
            for (int reach=radius-1;reach<=radius+2;++reach)
                root(game,plan,center+scale(dir,reach)+scale(side,bend));
        }
        std::vector<Cell> mouths(std::begin(directions),std::end(directions));shuffle(game,mouths);
        const int count=2+static_cast<int>(random_u32(game)%2);
        for (int i=0;i<count;++i) entrance(game,plan,tree,mouths[static_cast<std::size_t>(i)],static_cast<int>(random_u32(game)%5)-2);
        inner_roots(game,plan,tree,center);
        game.stage.roofs.push_back(roof);
        for (int y=-21;y<=21;++y) for (int x=-21;x<=21;++x) {
            const Cell c=center+Cell{x,y};if (!game.stage.in_bounds(c)) continue;
            plan.protected_cells[static_cast<std::size_t>(c.y*plan.width+c.x)]=1;
            if (tree_ellipse(roof,c,3) && walkable(game.stage.at_or_border(c))) tree.ground.push_back(c);
        }
    }
}

void populate_giant_tree(Game& game,const FloorPlan& plan,GenerationReport* report) {
    for (const auto& tree:plan.giant_trees) {
        auto ground=tree.ground;shuffle(game,ground);std::vector<Cell> occupied,nests;
        const int groups=4+static_cast<int>(random_u32(game)%3);
        for (Cell c:ground) {
            if (static_cast<int>(nests.size())>=groups) break;
            if (distance(c,tree.cache)<4) continue;
            bool spaced=true;for (Cell old:nests) if (distance(old,c)<8) spaced=false;
            if (spaced) nests.push_back(c);
        }
        for (Cell nest:nests) {
            const int count=3+static_cast<int>(random_u32(game)%3);int added=0;
            for (Cell c:ground) {
                if (added>=count) break;
                if (!vacant(game,c) || distance(c,nest)>4 || distance(c,tree.cache)<3) continue;
                bool spaced=true;for (Cell old:occupied) if (distance(old,c)<2) spaced=false;
                if (!spaced) continue;
                const auto kind=tree.spiders ? EntityKind::ForestSpider : occupied.empty() ? EntityKind::RootTurret :
                    random_u32(game)%4==0 ? EntityKind::BrambleGuard : EntityKind::ForagerGoblin;
                if (auto* actor=get_entity(game,spawn_entity(game,kind,c))) {
                    if (tree.spiders) set_forest_spider_role(*actor,occupied.empty() && game.run.floor>1 ? SpiderMother :
                        random_u32(game)%3==0 ? SpiderYoung : SpiderAdult);
                    occupied.push_back(c);++added;
                }
            }
        }
        // Child scenery rolls cluster around several independent interior sites.
        for (int group=0;group<10 && !ground.empty();++group) {
            const Cell anchor=ground[random_u32(game)%ground.size()];
            for (int j=0;j<8;++j) {
                const Cell c=anchor+Cell{static_cast<int>(random_u32(game)%7)-3,static_cast<int>(random_u32(game)%7)-3};
                if (!tree_ellipse(tree.canopy,c,3) || !vacant(game,c) || distance(c,tree.cache)<3) continue;
                const auto prop=tree.spiders && random_u32(game)%3==0 ? PropKind::ForestWeb :
                    random_u32(game)%4==0 ? PropKind::Puffball : random_u32(game)%2 ? PropKind::Fern : PropKind::Leaves;
                place_prop(game.stage,c,prop,static_cast<std::uint8_t>(random_u32(game)%3));
            }
        }
        compose_landmark_supplies(game,report,GenerationFeature::GiantTree,tree.ground,tree.cache);
        for (Cell d:{Cell{-6,-4},Cell{6,-4},Cell{0,6}}) if (game.run.roof_light_count<static_cast<int>(game.run.roof_lights.size()))
            game.run.roof_lights[static_cast<std::size_t>(game.run.roof_light_count++)]=
                {tree.canopy.start+Cell{tree.canopy.length/2,tree.canopy.width/2}+d,{10,1300,{199,211,141}}};
    }
}
