#include "giant_tree.hpp"
#include "tree_hollow.hpp"
#include "components.hpp"
#include "feature_roll.hpp"
#include "four_room_block.hpp"
#include "terrain_material.hpp"
#include "../scenery/hollow_tree.hpp"
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
    tree.cache=center;
    for(int attempt=0;attempt<16;++attempt) {
        const Cell at=center+Cell{static_cast<int>(random_u32(game)%23)-11,static_cast<int>(random_u32(game)%23)-11};
        if(tree_ellipse(tree.canopy,at,6) && distance(center,at)>=4){tree.cache=at;break;}
    }
    const WeightedComponent hollows[]{{OpenHeart,"Open heart",3},{RootGalleries,"Root galleries",5},{SplitHeart,"Split heart",4},{WetHollow,"Wet hollow",3},{RottenHeart,"Rotten heart",biome_stage(game.run.floor)>=2 ? 3U : 0U}};
    const auto hollow=roll_component(game,&plan.report,GenerationFeature::GiantTree,-1,"Hollow structure",center,hollows);
    tree.hollow=hollow.value;tree.hollow_component=hollow.record;
    tree.spiders=random_u32(game)%3==0;
    plan.giant_trees.push_back(tree);
    feature_reserved(plan,tree.rooms,std::string(hollows[tree.hollow].name)+(tree.spiders ? " / spider leaning" : " / mixed inhabitants"));
}

void carve_giant_tree(Game& game,FloorPlan& plan,GenerationTrace* trace) {
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
        compose_tree_hollow(game,plan,tree,trace);
        game.stage.roofs.push_back(roof);
        for (int y=-21;y<=21;++y) for (int x=-21;x<=21;++x) {
            const Cell c=center+Cell{x,y};if (!game.stage.in_bounds(c)) continue;
            plan.protected_cells[static_cast<std::size_t>(c.y*plan.width+c.x)]=1;
            if (tree_ellipse(roof,c,3) && walkable(game.stage.at_or_border(c))) tree.ground.push_back(c);
        }
    }
}
