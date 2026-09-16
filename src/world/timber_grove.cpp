#include "timber_grove.hpp"
#include "feature_roll.hpp"
#include "four_room_block.hpp"
#include "room_frame.hpp"
#include "terrain_material.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../props/interaction.hpp"
#include "../items/supply.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
Cell at(const TimberGrove& grove,Cell local) {return grove.center+turn_cell(local,grove.turns);}
void shuffle(Game& game,std::vector<Cell>& cells) {
    for (std::size_t i=cells.size();i>1;--i) std::swap(cells[i-1],cells[random_u32(game)%i]);
}
bool vacant(const Game& game,Cell cell) {
    const auto* tile=game.stage.at(cell);
    return tile && walkable(*tile) && tile->prop.kind==PropKind::None && entity_at(game,cell,false)<0;
}
void clear_cell(Game& game,Cell cell,TileKind kind) {if (auto* tile=game.stage.at(cell)) *tile={kind};}
bool near_break(const TimberGrove& grove,Cell cell,int radius) {
    for (Cell old:grove.firebreaks) if (distance(old,cell)<=radius) return true;
    return false;
}
void trails(Game& game,const FloorPlan& plan,TimberGrove& grove) {
    for (int y=-21;y<=21;++y) for (int x=-21;x<=21;++x) {
        const Cell cell=at(grove,{x,y});
        // A winding three-cell mineral strip splits the fuel beds. Route sockets
        // also stay bare, but their existing geometry still determines the paths.
        const int bend=grove.offset+x/6;
        const bool crossing=std::abs(y-bend)<=1;
        if (!crossing && !plan.protected_cell(cell)) continue;
        clear_cell(game,cell,TileKind::Ruin);grove.firebreaks.push_back(cell);
    }
    grove.entry=at(grove,{-20,grove.offset-3});
    grove.cache=at(grove,{17,grove.offset+2});
    // The approach's three-cell clearing holds tools; the far end holds a cache.
    for (Cell anchor:{grove.entry,grove.cache}) for (int y=-1;y<=1;++y) for (int x=-1;x<=1;++x) {
        const Cell cell=anchor+Cell{x,y};clear_cell(game,cell,TileKind::Ruin);grove.firebreaks.push_back(cell);
    }
}
void wet_refuge(Game& game,const FloorPlan& plan,TimberGrove& grove) {
    // A spring on either bank feeds a short shallow pool. It can quench actors
    // or refill their flask; it never takes over an existing required route.
    for (int sign:{1,-1}) {
        const Cell source=at(grove,{-18,sign*(11+static_cast<int>(random_u32(game)%4))});
        const Cell flow=turn_cell({1,0},grove.turns),side{-flow.y,flow.x};
        bool clear=true;
        for (int i=0;i<5;++i) for (int j=-2;j<=2;++j)
            if (plan.protected_cell(source+Cell{flow.x*i+side.x*j,flow.y*i+side.y*j})) clear=false;
        if (!clear) continue;
        grove.spring=source;
        for (int i=0;i<5;++i) for (int j=-2;j<=2;++j) {
            if ((i==0 || i==4) && std::abs(j)>1) continue;
            const Cell cell=source+Cell{flow.x*i+side.x*j,flow.y*i+side.y*j};
            clear_cell(game,cell,TileKind::ShallowWater);
            game.stage.at(cell)->current=static_cast<std::uint8_t>(grove.turns+1);
            grove.firebreaks.push_back(cell);
        }
        clear_cell(game,source,TileKind::Spring);
        game.stage.at(source)->current=static_cast<std::uint8_t>(grove.turns+1);
        break;
    }
}
void fuel_beds(Game& game,TimberGrove& grove) {
    std::vector<Cell> candidates;
    for (int y=-20;y<=20;++y) for (int x=-20;x<=20;++x) {
        const Cell cell=at(grove,{x,y});
        if (walkable(game.stage.at_or_border(cell)) && !near_break(grove,cell,0)) candidates.push_back(cell);
    }
    shuffle(game,candidates);
    const int wanted=18+static_cast<int>(random_u32(game)%13);
    for (Cell cell:candidates) {
        if (static_cast<int>(grove.trees.size())>=wanted) break;
        if (!vacant(game,cell) || near_break(grove,cell,4)) continue;
        bool spaced=true;for (Cell old:grove.trees) if (distance(old,cell)<4) spaced=false;
        if (!spaced) continue;
        // Falling in any direction stays away from the guaranteed bare routes.
        place_prop(game.stage,cell,PropKind::TallTree,static_cast<std::uint8_t>(random_u32(game)%4));
        grove.trees.push_back(cell);
    }
    struct Brush {Cell center;int radius;PropKind kind;};
    std::vector<Brush> patches;
    const int patch_count=10+static_cast<int>(random_u32(game)%7);
    for (int i=0;i<patch_count && !candidates.empty();++i) {
        const unsigned roll=random_u32(game)%10;
        patches.push_back({candidates[random_u32(game)%candidates.size()],3+static_cast<int>(random_u32(game)%4),
            roll<5 ? PropKind::TallGrass : roll<9 ? PropKind::Fern : PropKind::Puffball});
    }
    for (Cell cell:candidates) {
        if (!vacant(game,cell)) continue;
        // The adjoining fuel is mostly low litter. Separate child patches grow
        // taller plants with fuzzy edges, rather than repeating a grid of bushes.
        if (random_u32(game)%100>=84) continue;
        PropKind kind=random_u32(game)%4 ? PropKind::Leaves : PropKind::Twigs;
        for (const auto& patch:patches) {
            const Cell delta=cell-patch.center;const int squared=delta.x*delta.x+delta.y*delta.y;
            if (squared<patch.radius*patch.radius && random_u32(game)%100<70) kind=patch.kind;
        }
        place_prop(game.stage,cell,kind,static_cast<std::uint8_t>(random_u32(game)%3));
    }
    // Later ordinary scatter cannot erase the fuel breaks or spend this habitat's
    // population budget. Its own actors are placed in the surviving clear lanes.
    for (int y=-21;y<=21;++y) for (int x=-21;x<=21;++x) {
        const Cell cell=at(grove,{x,y});
        if (walkable(game.stage.at_or_border(cell))) grove.ground.push_back(cell);
    }
}
}

void plan_timber_grove(Game& game,FloorPlan& plan) {
    if (!roll_generation_feature(game,plan,GenerationFeature::TimberGrove)) return;
    TimberGrove grove;
    const auto center=reserve_four_rooms(game,plan,grove.rooms,&plan.report.features.back().candidate_count);
    if (!center) { feature_failed(plan,"No eligible four-room block; objectives and earlier habitats are excluded"); return; }
    grove.center=*center;grove.turns=static_cast<int>(random_u32(game)%4);
    grove.offset=static_cast<int>(random_u32(game)%7)-3;
    plan.timber_groves.push_back(grove);
    feature_reserved(plan,grove.rooms,"Rotation "+std::to_string(grove.turns)+" / offset "+std::to_string(grove.offset));
}

void carve_timber_grove(Game& game,FloorPlan& plan) {
    for (auto& grove:plan.timber_groves) {
        // Four independently sized lobes merge across former separating walls.
        // Uncarved margins stay wood, adding fuel and optional axe shortcuts.
        for (int y=-20;y<=20;++y) for (int x=-20;x<=20;++x) {
            const Cell cell=at(grove,{x,y});
            if (!plan.protected_cell(cell)) *game.stage.at(cell)=wood_tile(TileMaterial::Tree);
        }
        for (Cell corner:{Cell{-10,-10},Cell{10,-10},Cell{-10,10},Cell{10,10}}) {
            const int rx=12+static_cast<int>(random_u32(game)%3),ry=12+static_cast<int>(random_u32(game)%3);
            for (int y=-ry;y<=ry;++y) for (int x=-rx;x<=rx;++x) {
                const Cell local=corner+Cell{x,y};
                if (std::abs(local.x)>20 || std::abs(local.y)>20 || x*x*ry*ry+y*y*rx*rx>rx*rx*ry*ry) continue;
                clear_cell(game,at(grove,local),TileKind::Grass);
            }
        }
        trails(game,plan,grove);wet_refuge(game,plan,grove);fuel_beds(game,grove);
        for (int y=-21;y<=21;++y) for (int x=-21;x<=21;++x) {
            const Cell cell=at(grove,{x,y});
            plan.protected_cells[static_cast<std::size_t>(cell.y*plan.width+cell.x)]=1;
        }
    }
}

void populate_timber_grove(Game& game,const FloorPlan& plan) {
    for (const auto& grove:plan.timber_groves) {
        place_ground_item(game,grove.entry,ItemKind::Hatchet);
        place_ground_item(game,grove.entry+turn_cell({0,1},grove.turns),ItemKind::WaterFlask);
        place_ground_item(game,grove.entry+turn_cell({0,-1},grove.turns),ItemKind::Firecracker,3);
        const auto weapon=roll_item_supply(game,LootSource::Weapon,false);
        place_ground_item(game,grove.cache,weapon,supply_count(weapon));
        place_ground_item(game,grove.cache+turn_cell({1,0},grove.turns),ItemKind::ResinGlue,2);
        place_coins(game,grove.cache+turn_cell({0,1},grove.turns),30+static_cast<int>(random_u32(game)%26));
        auto ground=grove.ground;shuffle(game,ground);std::vector<Cell> occupied;
        const int wanted=12+static_cast<int>(random_u32(game)%9);
        for (Cell cell:ground) {
            if (static_cast<int>(occupied.size())>=wanted) break;
            if (distance(cell,grove.entry)<7 || distance(cell,grove.cache)<3 || entity_at(game,cell,false)>=0) continue;
            auto& tile=*game.stage.at(cell);
            if (!walkable(tile) || tile.kind!=TileKind::Grass) continue;
            bool spaced=true;for (Cell old:occupied) if (distance(old,cell)<4) spaced=false;
            if (!spaced) continue;
            tile.prop={};
            const unsigned roll=random_u32(game)%8;
            spawn_entity(game,roll<3 ? EntityKind::ForagerGoblin : roll<5 ? EntityKind::BrambleGuard :
                roll==5 ? EntityKind::RootTurret : roll==6 ? EntityKind::Woodpecker : EntityKind::Wolf,cell);
            occupied.push_back(cell);
        }
        for (Cell cell:{grove.entry,grove.cache}) if (game.run.roof_light_count<static_cast<int>(game.run.roof_lights.size()))
            game.run.roof_lights[static_cast<std::size_t>(game.run.roof_light_count++)]={cell,{12,1500,{211,194,136}}};
    }
}
