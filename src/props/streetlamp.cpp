#include "streetlamp.hpp"
#include "interaction.hpp"
#include <algorithm>
#include <vector>

namespace {
constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
bool quest_fixture(const Game& game,Cell cell) {
    for (const Entity& e:game.entities) {
        if (e.cell!=cell) continue;
        if (e.kind==EntityKind::Door || e.kind==EntityKind::Exit || e.kind==EntityKind::Switch ||
            e.kind==EntityKind::Key || e.kind==EntityKind::EncounterGate) return true;
    }
    return false;
}
}

bool streetlamp_ground(TileKind kind) {
    return buildable(kind) || kind==TileKind::Ruin || kind==TileKind::Rail ||
        kind==TileKind::Bridge || kind==TileKind::Ice;
}
// Prop stays eight bytes. variant is committed fall direction (E/S/W/N);
// growth_ticks counts 36 warning + 24 falling ticks, hp=1 during collapse.
// Repeated hits cannot accelerate the tell or reroll the impact direction.
Cell streetlamp_direction(const Prop& prop) {return directions[prop.variant&3U];}
bool hit_streetlamp(Game& game,Cell cell,int damage,Cell source) {
    Prop& prop=game.stage.at(cell)->prop;
    if (prop.growth_ticks>0) return true;
    if (damage<prop.hp) {
        prop.hp=static_cast<std::uint8_t>(prop.hp-damage);
        emit_sound(game,SoundId::PoleHit,cell);return true;
    }
    const Cell heading=cardinal_toward(source,cell,{1,0});
    for (int i=0;i<4;++i) if (directions[i]==heading) prop.variant=static_cast<std::uint8_t>(i);
    prop.hp=1;prop.growth_ticks=pole_warn_ticks+pole_fall_ticks;
    emit_sound(game,SoundId::PoleCreak,cell);return true;
}
void step_streetlamp(Game& game,Cell cell) {
    Prop& prop=game.stage.at(cell)->prop;
    if (prop.broken || prop.growth_ticks==0 || --prop.growth_ticks>0) return;
    const Cell direction=streetlamp_direction(prop);
    prop.hp=0;prop.broken=true;
    emit_sound(game,SoundId::PoleFall,cell);
    if (game.impact_count<static_cast<int>(game.impacts.size()))
        game.impacts[static_cast<std::size_t>(game.impact_count++)]={cell,cell-direction,Sprite::StreetLamp,32,true,PropKind::StreetLamp};
    for (int reach=1;reach<=pole_reach;++reach) {
        const Cell target=cell+Cell{direction.x*reach,direction.y*reach};
        Tile* tile=game.stage.at(target);
        if (!tile || !walkable(tile->kind) || quest_fixture(game,target)) break;
        std::vector<Handle> victims;
        for (int slot=0;slot<max_entities;++slot) {
            const Entity& actor=game.entities[static_cast<std::size_t>(slot)];
            if (actor.kind!=EntityKind::None && actor.cell==target && actor.impassable && actor.health>0)
                victims.push_back({slot,actor.generation});
        }
        hit_prop(game,target,40,cell);
        for (Handle victim:victims)
            if (const Entity* actor=get_entity(game,victim);actor && actor->cell==target)
                damage_entity(game,victim.slot,40,cell,false);
        // Cover absorbs the fall. Never bury a surviving body, item or fixture;
        // open ends and cuttable wreckage leave a way out after the impact.
        if (prop_blocks(tile->prop)) break;
        if (!streetlamp_ground(tile->kind) || entity_at(game,target,false)>=0 ||
            (tile->prop.kind!=PropKind::None && !tile->prop.broken)) continue;
        tile->prop={};place_prop(game.stage,target,PropKind::PoleWreck,direction.x!=0 ? 0 : 1);
    }
}
bool valid_streetlamp(const Prop& prop) {
    if (prop.kind!=PropKind::StreetLamp && prop.kind!=PropKind::PoleWreck) return true;
    if (prop.variant>3 || (!prop.broken && prop.hp==0)) return false;
    if (prop.kind==PropKind::PoleWreck) return prop.variant<=1 && prop.growth_ticks==0;
    return prop.growth_ticks<=pole_warn_ticks+pole_fall_ticks &&
        (prop.growth_ticks==0 || (!prop.broken && prop.hp==1));
}
