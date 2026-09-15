#include "hand_bellows.hpp"
#include "sled.hpp"
#include "../combat/shove.hpp"
#include "../entities/ash_sleeper.hpp"
#include "../surfaces/wind.hpp"
#include "../world/floating_items.hpp"
#include <algorithm>

namespace {
constexpr RegionalItem bellows{"Hand Bellows",
    "Eight puffs. Push a three-tile cone, drift smoke/spores forward, fan existing flames onto fuel. Walls stop pushes; hazards still hurt. Disturbs ash. Durable holds sixteen.",
    Sprite::HandBellows,{1,3,0,0,45,PatternEffect::Utility,false,1,0,false,true},
    ItemAction::Material,10,1,false,8,0,0,0,0,SoundId::BellowsPuff};
bool movable(const Game& game,const Entity& actor) {
    if (actor.hard_blocker || actor.toss.ticks || actor.vitals.rooted || actor.vitals.grip || ridden_sled(game,actor)) return false;
    if (actor.kind==EntityKind::GroundItem) return actor.ground_item.count>0 && actor.ground_item.flight.slot<0 && !sled_cargo(game,actor);
    return actor.health>0 && actor.move_interval>0 && actor.kind!=EntityKind::Train;
}
bool free_landing(const Game& game,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    if (!tile || !walkable(*tile)) return false;
    for (const Entity& other:game.entities)
        if (other.kind!=EntityKind::None && other.cell==cell && (other.impassable || other.kind==EntityKind::Sled || other.kind==EntityKind::GroundItem)) return false;
    return true;
}
}
const RegionalItem* hand_bellows_item(ItemKind kind) {return kind==ItemKind::HandBellows ? &bellows : nullptr;}
bool use_hand_bellows(Game& game,int slot,Cell direction) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    const Cell source=user.cell;const ItemPattern pattern=item_pattern(*user.inventory.held());
    if (user.inventory.held()->kind!=ItemKind::HandBellows || user.inventory.held()->uses<=0 || distance({},direction)!=1) return false;
    const auto cells=gust_cells(game,source,direction,pattern);
    struct Target {Handle handle;Cell cell;int reach;};
    std::vector<Target> targets;
    for (int index=0;index<max_entities;++index) {
        const Entity& actor=game.entities[static_cast<std::size_t>(index)];
        if (index==slot || actor.kind==EntityKind::None || !movable(game,actor) ||
            std::find(cells.begin(),cells.end(),actor.cell)==cells.end()) continue;
        const Cell offset=actor.cell-source;
        targets.push_back({{index,actor.generation},actor.cell,offset.x*direction.x+offset.y*direction.y});
    }
    std::stable_sort(targets.begin(),targets.end(),[](const Target& a,const Target& b){return a.reach>b.reach;});
    blow_surface_air(game,cells,direction);
    for (const Target& target:targets) {
        Entity* actor=get_entity(game,target.handle);
        if (!actor || actor->cell!=target.cell || !movable(game,*actor)) continue;
        rouse_ash_sleeper(game,*actor,source);
        if (!free_landing(game,actor->cell+direction)) continue;
        if (actor->kind==EntityKind::GroundItem) stop_item_float(game,*actor);
        shove_actor(game,target.handle.slot,direction,source);
    }
    if (game.sweep_count<static_cast<int>(game.sweeps.size()))
        game.sweeps[static_cast<std::size_t>(game.sweep_count++)]={source,direction,pattern.maximum,pattern.half_width,true};
    return true; // A real puff spends air even when only local debris is present.
}
