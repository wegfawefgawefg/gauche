#include "equipment_supply.hpp"
#include "ground_items.hpp"
#include "../items/supply.hpp"

void place_field_equipment(Game& game,const FloorPlan& plan,RoomSupplies& budget,ItemKind starter) {
    if (budget.equipment<=0) return;
    // Reserve a second practical tool before contextual room supplies spend the
    // budget. Explore the pre-key walking area; never hide it on a salvage island.
    std::vector<bool> seen(game.stage.tiles.size(),false),occupied(game.stage.tiles.size(),false),blocked(game.stage.tiles.size(),false);
    for (const Entity& e:game.entities) if (e.kind!=EntityKind::None && game.stage.in_bounds(e.cell)) {
        const auto index=static_cast<std::size_t>(e.cell.y*game.stage.width+e.cell.x);
        occupied[index]=true;blocked[index]=blocked[index] || (e.hard_blocker && e.impassable);
    }
    std::vector<Cell> queue{game.run.spawn},choices;
    constexpr Cell sides[]{{1,0},{-1,0},{0,1},{0,-1}};
    for (std::size_t next=0;next<queue.size();++next) {
        const Cell cell=queue[next];const Tile* tile=game.stage.at(cell);
        if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava ||
            tile->kind==TileKind::IceHole || cell==plan.door) continue;
        const auto index=static_cast<std::size_t>(cell.y*game.stage.width+cell.x);
        if (seen[index] || blocked[index]) continue;
        seen[index]=true;
        if (!occupied[index] && distance(cell,game.run.spawn)>=6 &&
            tile->kind!=TileKind::Spring && tile->kind!=TileKind::ShallowWater &&
            (tile->prop.kind==PropKind::None || tile->prop.broken)) choices.push_back(cell);
        for (Cell side:sides) queue.push_back(cell+side);
    }
    if (choices.empty()) return;
    const auto kind=roll_item_supply(game,LootSource::Cache,false,starter,SupplyNeed::Equipment);
    if (kind==ItemKind::None) return;
    auto* tally=budget.report ? &budget.report->supplies[static_cast<std::size_t>(kind)] : nullptr;
    if (tally) ++tally->attempted;
    const Cell cell=choices[random_u32(game)%choices.size()];
    if (get_entity(game,place_ground_item(game,cell,kind,supply_count(kind)))) {
        --budget.equipment;if (tally) ++tally->placed;
    } else if (tally) ++tally->rejected;
}
