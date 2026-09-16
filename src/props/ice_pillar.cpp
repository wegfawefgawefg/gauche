#include "ice_pillar.hpp"
#include "interaction.hpp"
#include "../surfaces/temperature.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>
#include <vector>

namespace {
constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
bool fixture(const Game& game,Cell cell) {
    for (const Entity& e:game.entities) if (e.cell==cell &&
        (e.kind==EntityKind::Key || e.kind==EntityKind::Switch || e.kind==EntityKind::Door ||
         e.kind==EntityKind::Exit || e.kind==EntityKind::EncounterGate)) return true;
    return false;
}
void fragments(Game& game,Cell cell,Cell source,PropKind kind) {
    if (game.impact_count<static_cast<int>(game.impacts.size()))
        game.impacts[static_cast<std::size_t>(game.impact_count++)]={cell,source,Sprite::IceRubble,12,true,kind};
}
void melt_water(Game& game,Cell cell) {
    Tile* tile=game.stage.at(cell);
    if (tile && (tile->surface.liquid==LiquidKind::None || tile->surface.liquid==LiquidKind::Water))
        pour_surface(game,cell,LiquidKind::Water,240);
}
void collapse(Game& game,Cell cell,Prop& prop) {
    const Cell dir=pillar_direction(prop);
    const int reach=pillar_reach(prop),damage=12+pillar_mass(prop)*3;
    prop.hp=0;prop.broken=true;
    emit_sound(game,SoundId::PillarFall,cell);fragments(game,cell,cell-dir,PropKind::IcePillar);
    for (int i=1;i<=reach;++i) {
        const Cell target=cell+Cell{dir.x*i,dir.y*i};Tile* tile=game.stage.at(target);
        if (!tile || !walkable(tile->kind) || fixture(game,target)) break;
        std::vector<Handle> victims;
        for (int slot=0;slot<max_entities;++slot) {
            const Entity& e=game.entities[static_cast<std::size_t>(slot)];
            if (e.kind!=EntityKind::None && e.cell==target && e.impassable && e.health>0)
                victims.push_back({slot,e.generation});
        }
        hit_prop(game,target,damage,cell);
        for (Handle victim:victims)
            if (const Entity* e=get_entity(game,victim);e && e->cell==target)
                damage_entity(game,victim.slot,damage,cell,false);
        fragments(game,target,cell,PropKind::IceRubble);
        if (prop_blocks(tile->prop)) break;
        // Survivors, loot and surviving floor props keep their cell; no rubble is
        // silently installed beneath them. Rubble elsewhere is breakable/meltable.
        if (entity_at(game,target,false)>=0 || surface_wet(*tile) || tile->kind==TileKind::Lava ||
            (tile->prop.kind!=PropKind::None && !tile->prop.broken)) continue;
        tile->prop={};place_prop(game.stage,target,PropKind::IceRubble,dir.x==0 ? 1 : 0);
    }
}
}
Cell pillar_direction(const Prop& prop) {return directions[prop.variant&3U];}
int pillar_height(const Prop& prop) {constexpr int heights[]{3,2,4,5};return heights[prop.variant>>6];}
int pillar_mass(const Prop& prop) {return std::max(0,pillar_melt_steps-static_cast<int>((prop.variant>>2)&15U));}
// The tip must reach a cell's near edge before that cell is in the crush lane.
int pillar_reach(const Prop& prop) {return (pillar_mass(prop)*pillar_height(prop)+pillar_melt_steps/2)/pillar_melt_steps;}
bool hit_ice_pillar(Game& game,Cell cell,int damage,Cell source) {
    Prop& prop=game.stage.at(cell)->prop;
    if (prop.growth_ticks>0) return true;
    if (damage<prop.hp) {
        prop.hp=static_cast<std::uint8_t>(prop.hp-damage);emit_sound(game,SoundId::IceBlockHit,cell);return true;
    }
    const Cell heading=cardinal_toward(source,cell,{1,0});
    for (int i=0;i<4;++i) if (directions[i]==heading)
        prop.variant=static_cast<std::uint8_t>((prop.variant&~3U)|static_cast<unsigned int>(i));
    prop.hp=1;prop.growth_ticks=pillar_warn_ticks+pillar_fall_ticks;
    emit_sound(game,SoundId::PillarCrack,cell);return true;
}
void step_ice_pillar(Game& game,Cell cell) {
    Prop& prop=game.stage.at(cell)->prop;
    if (prop.broken) return;
    if (prop.kind==PropKind::IcePillar && prop.growth_ticks>0) {
        if (--prop.growth_ticks==0) collapse(game,cell,prop);
        return; // The committed fall cannot be cancelled or shortened by more hits/heat.
    }
    if (game.tick%30!=0 || !warm_cell(game,cell)) return;
    if (prop.kind==PropKind::IceRubble) {
        prop.hp=static_cast<std::uint8_t>(std::max(0,static_cast<int>(prop.hp)-4));
    } else {
        prop.variant=static_cast<std::uint8_t>(prop.variant+4);
        if (pillar_mass(prop)==0) prop.hp=0;
    }
    // Drips can quench the heat source. Sustained fire melts the full pillar.
    melt_water(game,cell);
    if (prop.kind==PropKind::IcePillar && pillar_mass(prop)%3==0)
        for (Cell dir:directions) melt_water(game,cell+dir);
    if (prop.hp==0) {prop.broken=true;emit_sound(game,SoundId::PillarMelt,cell);}
    else emit_sound(game,SoundId::RoofMelt,cell);
}
bool valid_ice_pillar(const Prop& prop) {
    if (prop.kind==PropKind::IceRubble)
        return prop.variant<=1 && prop.growth_ticks==0 && (prop.broken || prop.hp>0);
    if (prop.kind!=PropKind::IcePillar) return true;
    return ((prop.variant>>2)&15U)<=12 && (prop.broken || (prop.hp>0 && pillar_mass(prop)>0)) &&
        prop.growth_ticks<=pillar_warn_ticks+pillar_fall_ticks &&
        (prop.growth_ticks==0 || (!prop.broken && prop.hp==1));
}
