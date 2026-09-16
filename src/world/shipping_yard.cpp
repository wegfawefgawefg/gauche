#include "shipping_yard.hpp"
#include "roof_scenes.hpp"
#include "../scenery/roof.hpp"
#include "../props/interaction.hpp"
#include "../items/supply.hpp"

namespace {
void reserve_passage(FloorPlan& plan,const RoofSpan& roof) {
    // Later dressing must not plug the walk-through passage or either doorway.
    for (int along=-1;along<=roof.length;++along) {
        const Cell cell=roof_cell(roof,along,1);
        plan.protected_cells[static_cast<std::size_t>(cell.y*plan.width+cell.x)]=1;
    }
}
void cargo(Game& game,const RoomPlan& room,const RoofSpan& roof) {
    const Cell deck=roof_cell(roof,roof.length/2,1);
    place_prop(game.stage,deck,PropKind::Pallet);
    // Relocate existing equipment, never machine fuel or a new extra loot roll.
    for (auto& actor:game.entities) {
        if (actor.kind!=EntityKind::GroundItem || distance(actor.cell,room.center)>room.half_width) continue;
        bool stored=false;
        for (const auto& other:game.stage.roofs) if (roof_covers(other,actor.cell)) stored=true;
        if (stored) continue;
        const auto& entry=item_supply(actor.ground_item.kind);
        if (entry.role!=ItemRole::Combat && entry.role!=ItemRole::Survival) continue;
        actor.cell=deck;return;
    }
}
}

void place_shipping_containers(Game& game,FloorPlan& plan) {
    if (game.run.layout!=FloorLayout::Generated || floor_biome(game.run.floor)!=Biome::Industrial || plan.rooms.empty()) return;
    const auto offset=random_u32(game)%plan.rooms.size();
    int placed=0;
    for (std::size_t i=0;i<plan.rooms.size() && placed<2;++i) {
        const auto& room=plan.rooms[(offset+i)%plan.rooms.size()];
        if (room.role==RoomRole::Entrance || room.role==RoomRole::Exit || room.role==RoomRole::Shrine ||
            room.role==RoomRole::Secret) continue;
        for (int attempt=0;attempt<70;++attempt) {
            RoofSpan roof;roof.kind=RoofKind::Container;roof.hp=80;
            roof.length=static_cast<std::uint8_t>(5+random_u32(game)%3);
            roof.vertical=static_cast<std::uint8_t>(random_u32(game)%2);
            roof.start=room.center+Cell{
                static_cast<int>(random_u32(game)%static_cast<unsigned>(room.half_width*2+1))-room.half_width,
                static_cast<int>(random_u32(game)%static_cast<unsigned>(room.half_height*2+1))-room.half_height};
            if (!place_roof_span(game,plan,roof)) continue;
            reserve_passage(plan,roof);cargo(game,room,roof);++placed;
            // A second parallel shell leaves one tile of open yard between them.
            RoofSpan neighbor=roof;neighbor.start=neighbor.start+(roof.vertical ? Cell{4,0} : Cell{0,4});
            if (placed<2 && place_roof_span(game,plan,neighbor)) {reserve_passage(plan,neighbor);cargo(game,room,neighbor);++placed;}
            break;
        }
    }
}
