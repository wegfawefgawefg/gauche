#include "reactor.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../props/interaction.hpp"

namespace {
void fill(Stage& stage,Cell low,Cell high,TileKind kind) {
    for (int y=low.y;y<=high.y;++y) for (int x=low.x;x<=high.x;++x)
        *stage.at({x,y})={kind,static_cast<std::uint16_t>(kind==TileKind::Wall ? 80 : 0),0,80};
}
void prop(Game& game,Cell cell,PropKind kind) {place_prop(game.stage,cell,kind,0);}
}

bool make_last_shift(Game& game,bool force) {
    if (!force && (game.run.floor!=12 || random_u32(game)%5!=0)) return false;
    Stage& stage=game.stage;stage.width=64;stage.height=48;
    stage.tiles.assign(64*48,{TileKind::Wall,80,0,80});
    // Two service corridors loop around a broad reactor hall. Side rooms reward
    // clearing and scouting before the irreversible shutdown, or greed afterward.
    fill(stage,{7,6},{11,43},TileKind::Ruin);
    fill(stage,{51,6},{55,43},TileKind::Ruin);
    fill(stage,{7,6},{55,10},TileKind::Ruin);
    fill(stage,{7,38},{55,43},TileKind::Ruin);
    fill(stage,{22,19},{41,35},TileKind::Ruin);
    fill(stage,{9,25},{54,29},TileKind::Ruin);
    fill(stage,{30,33},{34,41},TileKind::Ruin);
    fill(stage,{14,12},{25,21},TileKind::Ruin);
    fill(stage,{18,9},{20,13},TileKind::Ruin);
    fill(stage,{38,12},{49,21},TileKind::Ruin);
    fill(stage,{44,9},{46,13},TileKind::Ruin);
    fill(stage,{14,31},{20,36},TileKind::Ruin);
    fill(stage,{18,27},{20,32},TileKind::Ruin);
    fill(stage,{44,31},{49,36},TileKind::Ruin);
    fill(stage,{44,27},{46,32},TileKind::Ruin);
    // The upper security seam cannot be mined around. All other partitions
    // remain ordinary tool-breakable rock, including shortcuts out of the hall.
    fill(stage,{29,1},{35,3},TileKind::Ruin);
    for (int x=0;x<64;++x) for (int y=0;y<6;++y)
        if (stage.at({x,y})->kind==TileKind::Wall)
            *stage.at({x,y})={TileKind::Wall,100,0,100,BreakRule::Unbreakable,0};
    fill(stage,{32,4},{32,6},TileKind::Ruin);
    for (int y=0;y<48;++y) for (int x=0;x<64;++x)
        if (x==0 || y==0 || x==63 || y==47)
            *stage.at({x,y})={TileKind::Wall,100,0,100,BreakRule::Unbreakable,0};
    // Refill pools form useful cold interruptions without flooding either exit route.
    fill(stage,{15,13},{17,16},TileKind::ShallowWater);
    fill(stage,{47,32},{48,34},TileKind::ShallowWater);
    game.run.layout=FloorLayout::LastShift;game.run.spawn={29,41};game.run.exit={32,2};
    game.run.objective=ObjectiveKind::Switch;game.run.has_key=false;
    game.run.roof_lights={};game.run.roof_light_count=0;
    return true;
}

void populate_last_shift(Game& game) {
    Entity& core=*get_entity(game,spawn_entity(game,EntityKind::Switch,{32,28}));
    core.sprite=Sprite::ReactorCore;core.impassable=core.hard_blocker=true;
    core.light={8,900,{133,181,82}};core.self_light={126,151,91};
    spawn_entity(game,EntityKind::Door,{32,4});spawn_entity(game,EntityKind::Exit,game.run.exit);
    // Marked vent housings frame the control, leaving a clear approach from south.
    for (Cell cell:{Cell{26,21},Cell{38,21},Cell{26,32},Cell{38,32}})
        prop(game,cell,PropKind::Grate);
    for (Cell cell:{Cell{8,8},Cell{54,8},Cell{8,39},Cell{54,39},Cell{29,20},Cell{35,20}})
        prop(game,cell,PropKind::BeamLamp);
    for (Cell cell:{Cell{16,19},Cell{48,19},Cell{15,35},Cell{48,35}})
        prop(game,cell,PropKind::MaintenanceLocker);
    for (Cell cell:{Cell{14,20},Cell{49,20},Cell{14,34},Cell{49,31}})
        prop(game,cell,PropKind::ScrapBin);
    spawn_entity(game,EntityKind::RivetGunner,{44,17});
    spawn_entity(game,EntityKind::Strikebreaker,{20,17});
    spawn_entity(game,EntityKind::AshSleeper,{16,34});
    spawn_entity(game,EntityKind::PressureRat,{47,35});
    spawn_entity(game,EntityKind::WalkingKiln,{38,24});
    spawn_entity(game,EntityKind::SlagSnail,{24,24});
    // Useful native supplies; escape never depends on consuming a specific item.
    place_ground_item(game,{29,39},ItemKind::CoolantCan);
    place_ground_item(game,{34,39},ItemKind::EmergencyFoam,2);
    place_ground_item(game,{16,17},ItemKind::PocketPump);
    place_ground_item(game,{31,43},ItemKind::LunchTin);
    place_ground_item(game,{18,33},ItemKind::PocketDrill);
    place_ground_item(game,{46,15},ItemKind::RivetGun);
    for (Cell cell:{Cell{23,14},Cell{40,14},Cell{15,33},Cell{48,33}}) place_coins(game,cell,12);
}
