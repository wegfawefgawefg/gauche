#include "industrial_geometry.hpp"
#include "ground_items.hpp"
#include "../props/interaction.hpp"
#include "../props/conveyor.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
Cell times(Cell cell,int count) {return {cell.x*count,cell.y*count};}
bool adaptable(RoomRole role) {
    switch (role) {
    case RoomRole::Clearing: case RoomRole::Thicket: case RoomRole::Brook: case RoomRole::Ruins:
    case RoomRole::Den: case RoomRole::Cache: case RoomRole::Workshop: case RoomRole::Orchard:
    case RoomRole::Workfront: case RoomRole::BlastingAlcove: case RoomRole::AssemblyLine: return true;
    default: return false;
    }
}
void reserve(FloorPlan& plan,Cell cell) {
    if (cell.x>=0 && cell.y>=0 && cell.x<plan.width && cell.y<plan.height)
        plan.protected_cells[static_cast<std::size_t>(cell.y*plan.width+cell.x)]=1;
}
void floor(Game& game,FloorPlan& plan,Cell cell,bool protect=true) {
    if (auto* tile=game.stage.at(cell)) {*tile={TileKind::Ruin};if (protect) reserve(plan,cell);}
}
void line(std::vector<Cell>& cells,Cell from,Cell to) {
    if (cells.empty() || cells.back()!=from) cells.push_back(from);
    while (from!=to) {from=from+cardinal_toward(from,to,{1,0});cells.push_back(from);}
}
bool gate_clear(const FloorPlan& plan,RouteEdge edge,Cell across) {
    for (auto link:plan.edges) for (int index:{edge.a,edge.b}) {
        const int neighbor=link.a==index ? link.b : link.b==index ? link.a : -1;
        if (neighbor<0 || (neighbor!=plan.exit_room && neighbor!=plan.secret_room)) continue;
        const Cell side=plan.rooms[static_cast<std::size_t>(neighbor)].grid-
            plan.rooms[static_cast<std::size_t>(index)].grid;
        if (side.x*across.x+side.y*across.y>0) return false;
    }
    return true;
}
}

void plan_industrial_geometry(Game& game,FloorPlan& plan) {
    if (!industrial_floor(game.run.floor)) return;
    plan.industry_profile=1+static_cast<int>(random_u32(game)%2);
    for (auto& room:plan.rooms) if (adaptable(room.role)) {
        room.shape=plan.industry_profile==1 ? RoomShape::WorkHall : RoomShape::ExcavatedHall;
        room.half_width=10;room.half_height=9;
    }
    std::vector<IndustrialLink> choices;
    for (auto edge:plan.edges) {
        const auto& a=plan.rooms[static_cast<std::size_t>(edge.a)];
        const auto& b=plan.rooms[static_cast<std::size_t>(edge.b)];
        if (!adaptable(a.role) || !adaptable(b.role)) continue;
        const Cell along=cardinal_toward(a.center,b.center,{1,0});
        for (int sign:{-1,1}) {
            const Cell across=times({-along.y,along.x},sign);
            if (!gate_clear(plan,edge,across)) continue;
            IndustrialLink link;link.a=edge.a;link.b=edge.b;link.along=along;link.across=across;
            choices.push_back(link);
            if (a.role==RoomRole::Workfront || a.role==RoomRole::AssemblyLine ||
                b.role==RoomRole::Workfront || b.role==RoomRole::AssemblyLine) choices.push_back(link);
        }
    }
    if (!choices.empty()) plan.industrial_links.push_back(choices[random_u32(game)%choices.size()]);
}

void carve_industrial_geometry(Game& game,FloorPlan& plan) {
    for (auto& link:plan.industrial_links) {
        const Cell a=plan.rooms[static_cast<std::size_t>(link.a)].center;
        const Cell b=plan.rooms[static_cast<std::size_t>(link.b)].center;
        const Cell along=link.along,across=link.across;
        link.middle={(a.x+b.x)/2,(a.y+b.y)/2};
        const Cell turn=along.x ? Cell{b.x,a.y} : Cell{a.x,b.y};
        std::vector<Cell> spine;line(spine,a,turn);line(spine,turn,b);
        for (Cell center:spine) for (int width=-8;width<=8;++width) {
            const Cell cell=center+times(across,width);
            // Reserve the shared service floor; original room interiors outside
            // the connection retain their installation and clutter space.
            floor(game,plan,cell);
        }
        for (int length=-3;length<=3;++length) for (int width=-7;width<=7;++width) {
            const Cell cell=link.middle+times(along,length)+times(across,width);
            Tile* tile=game.stage.at(cell);if (!tile) continue;
            if (plan.industry_profile==1 && std::abs(length)<=2 && std::abs(width)<=3)
                *tile={TileKind::Wall,120,0,120,BreakRule::DigRequired,2};
            else if (plan.industry_profile==1 && width==-7)
                *tile={TileKind::Lava};
            else if (plan.industry_profile==2 && width<=-2 && (width>-7 || length%2==0))
                *tile={TileKind::Chasm};
            else if (plan.industry_profile==2 && width>=2 && width<=3)
                *tile={TileKind::Wall,75,0,75,BreakRule::DigRequired,1};
            reserve(plan,cell);
        }
        // Two bends on either side of the shared wall make the route visible.
        // Leave a dry tile after the last belt: cargo queues there, never vanishes.
        link.load=a+times(across,7)-times(along,4);
        const Cell first=a+times(across,7)+times(along,6);
        const Cell second=first-times(across,2);
        const Cell third=b+times(across,5)-times(along,6);
        const Cell fourth=third+times(across,2);
        link.unload=b+times(across,7)+times(along,5);
        const Cell midturn=along.x ? Cell{third.x-along.x*2,second.y} : Cell{second.x,third.y-along.y*2};
        const Cell adjust=along.x ? Cell{midturn.x,third.y} : Cell{third.x,midturn.y};
        line(link.belt,link.load,first);line(link.belt,first,second);
        line(link.belt,second,midturn);line(link.belt,midturn,adjust);line(link.belt,adjust,third);
        line(link.belt,third,fourth);line(link.belt,fourth,link.unload);
        for (std::size_t i=0;i<link.belt.size();++i) {
            const Cell cell=link.belt[i];floor(game,plan,cell);
            if (i+1==link.belt.size()) break;
            const Cell direction=link.belt[i+1]-cell;
            const auto variant=static_cast<std::uint8_t>(direction.x>0 ? 0 : direction.y>0 ? 1 : direction.x<0 ? 2 : 3);
            place_prop(game.stage,cell,PropKind::Conveyor,variant);
        }
        const Cell machine=link.load+across;
        floor(game,plan,machine);
        const Cell face=machine+across;
        if (auto* ore=game.stage.at(face)) {
            *ore={TileKind::Wall,480,0,480,BreakRule::DigRequired,1};
            ore->contents=ItemKind::CoalLump;ore->content_count=16;reserve(plan,face);
        }
        const Cell mount=link.unload+along;
        floor(game,plan,mount);
        floor(game,plan,mount+along);place_prop(game.stage,mount+along,PropKind::SteamDrive);
        floor(game,plan,mount+across);floor(game,plan,mount+along+across);
        for (int i=1;i<=3;++i) {
            const Cell cell=mount-times(across,i);floor(game,plan,cell);
            if (i==3) *game.stage.at(cell)={TileKind::Spring};
            else place_prop(game.stage,cell,PropKind::WaterPipe,static_cast<std::uint8_t>(across.y!=0));
        }
        // Keep both ends approachable without stepping onto the moving lane.
        for (Cell end:{link.load,link.unload}) for (int side:{-1,1}) floor(game,plan,end+times(across,side));
    }
}

void populate_industrial_links(Game& game,const FloorPlan& plan) {
    for (const auto& link:plan.industrial_links) {
        const Handle cutter_handle=spawn_entity(game,EntityKind::CoalCutter,link.load+link.across);
        if (Entity* cutter=get_entity(game,cutter_handle))
            cutter->facing=link.across;
        const Cell mount=link.unload+link.along;
        const Handle tank_handle=spawn_entity(game,EntityKind::BoilerTank,mount);
        if (Entity* tank=get_entity(game,tank_handle)) {
            tank->counter_b=0;tank->facing=link.along;
            BoilerFeed feed{tank_handle,mount,mount-times(link.across,3),link.unload,600,0};
            feed.drive=mount+link.along;feed.cutter=cutter_handle;
            feed.belts.assign(link.belt.begin(),link.belt.end()-1);game.boiler_feeds.push_back(feed);
        }
        place_ground_item(game,link.load-link.along,ItemKind::BoltPouch,3);
        place_ground_item(game,mount+link.across,ItemKind::Sealant);
        place_ground_item(game,mount+link.along+link.across,ItemKind::BeltCrank);
        // Two of the initial eight coal are already at the hopper: the line
        // starts cold, then warms itself without requiring a powered fuel belt.
        place_ground_item(game,link.unload,ItemKind::CoalLump,2);
        constexpr ItemKind cargo[]{ItemKind::CoalLump,ItemKind::CoalLump};
        for (std::size_t i=0;i<std::size(cargo);++i) {
            const Cell cell=link.belt[i*6];
            if (live_belt(game.stage.at_or_border(cell).prop)) place_ground_item(game,cell,cargo[i],i==0 ? 2 : 4);
        }
        place_ground_item(game,link.load-link.across,ItemKind::BrakeShoe);
        for (Cell end:{link.load,link.unload}) {
            const Cell bin=end+times(link.across,2);
            const Cell lamp=end-times(link.across,2);
            if (end!=link.load && walkable(game.stage.at_or_border(bin))) place_prop(game.stage,bin,PropKind::OreBin);
            if (walkable(game.stage.at_or_border(lamp))) place_prop(game.stage,lamp,PropKind::StreetLamp);
        }
    }
}
