#include "freight_exchange.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../props/interaction.hpp"
#include "../props/conveyor.hpp"

namespace {
void fill(Stage& stage,Cell low,Cell high,TileKind kind) {
    for (int y=low.y;y<=high.y;++y) for (int x=low.x;x<=high.x;++x)
        *stage.at({x,y})={kind,static_cast<std::uint16_t>(kind==TileKind::Wall ? 80 : 0),0};
}
void track(Stage& stage,Cell from,Cell to) {
    const Cell step{from.x==to.x ? 0 : 1,from.y==to.y ? 0 : 1};
    for (;;) {
        *stage.at(from)={TileKind::Rail,24,0,24,BreakRule::DigRequired,1};
        if (from==to) return;
        from=from+step;
    }
}
void freight_pair(Game& game,Cell cell,Cell facing,Item load) {
    const Handle cart=spawn_entity(game,EntityKind::RailCart,cell);
    Entity& tub=*get_entity(game,cart);tub.facing=facing;tub.ground_item=load;
    Entity& shunter=*get_entity(game,spawn_entity(game,EntityKind::RailShunter,cell-facing));
    shunter.entity_a=cart;shunter.facing=facing;
}
void fixture(Game& game,Cell cell,PropKind kind,int variant=0) {
    place_prop(game.stage,cell,kind,static_cast<std::uint8_t>(variant));
}
}

bool make_freight_exchange(Game& game,bool force) {
    // One eligible stage gives a run-wide cap naturally; debug forcing is separate.
    if (!force && (game.run.floor!=10 || random_u32(game)%5!=0)) return false;
    game.stage.width=64;game.stage.height=48;
    game.stage.tiles.assign(64*48,{TileKind::Wall,80,0});
    fill(game.stage,{28,6},{36,44},TileKind::Ruin);
    fill(game.stage,{5,8},{58,16},TileKind::Ruin);
    fill(game.stage,{7,6},{18,18},TileKind::Ruin);
    fill(game.stage,{46,6},{58,18},TileKind::Ruin);
    fill(game.stage,{5,22},{58,30},TileKind::Ruin);
    fill(game.stage,{5,35},{32,43},TileKind::Ruin);
    fill(game.stage,{10,16},{12,36},TileKind::Ruin);
    fill(game.stage,{49,16},{51,30},TileKind::Ruin);
    // Loading halls and control booth: broad crossings, smaller side doors,
    // and ordinary diggable interior partitions rather than an empty arena.
    for (int y:{19,33}) {
        fill(game.stage,{3,y},{25,y},TileKind::Wall);
        fill(game.stage,{39,y},{60,y},TileKind::Wall);
        fill(game.stage,{10,y},{12,y},TileKind::Ruin);
        if (y==19) fill(game.stage,{49,y},{51,y},TileKind::Ruin);
    }
    fill(game.stage,{20,6},{20,18},TileKind::Wall);
    fill(game.stage,{44,6},{44,18},TileKind::Wall);
    fill(game.stage,{20,10},{20,12},TileKind::Ruin);
    fill(game.stage,{44,14},{44,16},TileKind::Ruin);
    // The exit has one walking entrance. Machine/tool earned interior routes
    // cannot accidentally excavate another approach through the quest seal.
    fill(game.stage,{28,1},{36,3},TileKind::Ruin);
    for (int x=0;x<64;++x)
        *game.stage.at({x,4})={TileKind::Wall,100,0,100,BreakRule::Unbreakable,0};
    fill(game.stage,{32,4},{32,6},TileKind::Ruin);
    for (int y=0;y<48;++y) for (int x=0;x<64;++x)
        if (x==0 || y==0 || x==63 || y==47)
            *game.stage.at({x,y})={TileKind::Wall,100,0,100,BreakRule::Unbreakable,0};
    track(game.stage,{8,11},{55,11});track(game.stage,{8,25},{55,25});
    track(game.stage,{32,11},{32,25});
    // Small optional hot sorting corner; both halls retain dry side routes.
    fill(game.stage,{18,35},{19,36},TileKind::Lava);
    game.run.layout=FloorLayout::FreightExchange;
    game.run.spawn={8,40};game.run.exit={32,2};game.run.has_key=false;
    game.run.objective=ObjectiveKind::Switch;
    game.run.roof_lights={};game.run.roof_light_count=0;
    return true;
}

void populate_freight_exchange(Game& game) {
    spawn_entity(game,EntityKind::Switch,{53,9});
    spawn_entity(game,EntityKind::Door,{32,4});
    spawn_entity(game,EntityKind::Exit,game.run.exit);
    fixture(game,{32,25},PropKind::RailPoints,0);
    fixture(game,{32,11},PropKind::RailPoints,2);
    for (Cell cell:{Cell{10,9},Cell{33,13},Cell{51,9},Cell{29,24},Cell{49,27},Cell{12,39}})
        fixture(game,cell,cell.x==29 || cell.x==51 ? PropKind::StreetLamp : PropKind::BeamLamp);
    Item hammer=make_item(ItemKind::PressHammer);hammer.durability=12;
    freight_pair(game,{9,25},{1,0},hammer);
    freight_pair(game,{54,11},{-1,0},make_item(ItemKind::BoltPouch,3));
    // Crane and gunner work at opposite ends of the upper loading hall. They
    // are not encounter gates: slipping past, hauling cargo or digging works.
    spawn_entity(game,EntityKind::MagnetCrane,{13,14});
    Entity& gunner=*get_entity(game,spawn_entity(game,EntityKind::RivetGunner,{49,15}));
    gunner.facing={-1,0};
    for (Cell cell:{Cell{47,13},Cell{48,13},Cell{47,17},Cell{48,17}}) fixture(game,cell,PropKind::Grate);
    spawn_entity(game,EntityKind::Counterweight,{49,28});
    place_coins(game,{49,29},8);
    place_coins(game,{53,8},10);
    for (Cell cell:{Cell{7,15},Cell{17,15},Cell{42,29},Cell{54,30},Cell{23,37}})
        fixture(game,cell,PropKind::ScrapBin);
    fixture(game,{55,15},PropKind::OreBin);
    fixture(game,{8,37},PropKind::Crate);
    for (int x=11;x<=17;++x) fixture(game,{x,36},PropKind::Conveyor,belt_manual);
    place_ground_item(game,{12,36},ItemKind::NailBoard);
    place_ground_item(game,{10,36},ItemKind::BeltCrank);
    // Cranking the sorting belt wakes the sleeper; quiet retrieval is possible.
    spawn_entity(game,EntityKind::AshSleeper,{16,38});
    place_ground_item(game,{10,40},ItemKind::RailSwitchKey);
    place_ground_item(game,{8,42},ItemKind::ChainHook);
    place_ground_item(game,{12,40},ItemKind::RubberMallet);
    place_ground_item(game,{7,41},ItemKind::Bandage,2);
    // Actual worn salvage attracts the crane; the supplied hook can recover it.
    const Handle scrap=spawn_entity(game,EntityKind::GroundItem,{13,12});
    Entity& pick=*get_entity(game,scrap);pick.ground_item=make_item(ItemKind::Pickaxe);
    pick.ground_item.uses=6;pick.sprite=Sprite::Pickaxe;
}
