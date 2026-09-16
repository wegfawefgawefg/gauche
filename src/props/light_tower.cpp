#include "light_tower.hpp"
#include "interaction.hpp"
#include "streetlamp.hpp"
#include <algorithm>
#include <vector>

namespace {
constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
bool fixture(const Game& game,Cell cell) {
    for (const Entity& actor:game.entities) if (actor.cell==cell &&
        (actor.kind==EntityKind::Key || actor.kind==EntityKind::Switch || actor.kind==EntityKind::Door ||
         actor.kind==EntityKind::Exit || actor.kind==EntityKind::EncounterGate)) return true;
    return false;
}
void fragments(Game& game,Cell cell,Cell source,PropKind kind) {
    if (game.impact_count<static_cast<int>(game.impacts.size()))
        game.impacts[static_cast<std::size_t>(game.impact_count++)]={cell,source,Sprite::TowerWreck,24,true,kind};
}
void collapse(Game& game,Cell root,Prop& tower) {
    const Cell dir=tower_direction(tower);tower.hp=0;tower.broken=true;
    emit_sound(game,SoundId::TowerCrash,root);fragments(game,root,root-dir,PropKind::LightTower);
    for (int n=1;n<=tower_reach;++n) {
        const Cell cell=root+Cell{dir.x*n,dir.y*n};Tile* tile=game.stage.at(cell);
        if (!tile || !walkable(tile->kind) || fixture(game,cell)) break;
        std::vector<Handle> victims;
        for (int slot=0;slot<max_entities;++slot) {
            const auto& actor=game.entities[static_cast<std::size_t>(slot)];
            if (actor.kind!=EntityKind::None && actor.cell==cell && actor.impassable && actor.health>0)
                victims.push_back({slot,actor.generation});
        }
        // Pipes, belts and other destructible installations take the real blow.
        hit_prop(game,cell,60,root);
        for (Handle h:victims) if (const auto* actor=get_entity(game,h);actor && actor->cell==cell)
            damage_entity(game,h.slot,60,root,false);
        fragments(game,cell,root,PropKind::TowerWreck);
        if (prop_blocks(tile->prop)) break;
        if (!streetlamp_ground(tile->kind) || entity_at(game,cell,false)>=0 ||
            (tile->prop.kind!=PropKind::None && !tile->prop.broken)) continue;
        // Keep broken machine fixtures identifiable/repairable; never turn a
        // real pipe or driven belt into an unrelated piece of scenery.
        if (tile->prop.kind==PropKind::WaterPipe || tile->prop.kind==PropKind::SteamDrive ||
            tile->prop.kind==PropKind::Conveyor || bridge_prop(tile->prop.kind)) continue;
        tile->prop={};place_prop(game.stage,cell,PropKind::TowerWreck,dir.y!=0 ? 1 : 0);
    }
}
}
Cell tower_direction(const Prop& prop) {return directions[prop.variant&3U];}
bool hit_light_tower(Game& game,Cell cell,int damage) {
    Prop& tower=game.stage.at(cell)->prop;
    if (tower.growth_ticks>0) return true;
    if (damage<tower.hp) {
        tower.hp=static_cast<std::uint8_t>(tower.hp-damage);
        emit_sound(game,SoundId::PoleHit,cell);return true;
    }
    tower.hp=1;tower.growth_ticks=tower_warn_ticks+tower_fall_ticks;
    emit_sound(game,SoundId::TowerGroan,cell);return true;
}
void step_light_tower(Game& game,Cell cell) {
    Prop& tower=game.stage.at(cell)->prop;
    if (tower.broken || tower.growth_ticks==0) return;
    --tower.growth_ticks;
    if (tower.growth_ticks==tower_fall_ticks) {
        emit_sound(game,SoundId::LampBreak,cell);fragments(game,cell,cell,PropKind::LightTower);
    }
    if (tower.growth_ticks==0) collapse(game,cell,tower);
}
bool valid_light_tower(const Prop& prop) {
    if (prop.kind==PropKind::TowerWreck)
        return prop.variant<=1 && prop.growth_ticks==0 && (prop.broken || prop.hp>0);
    if (prop.kind!=PropKind::LightTower) return true;
    return prop.variant<4 && (prop.broken || prop.hp>0) &&
        prop.growth_ticks<=tower_warn_ticks+tower_fall_ticks &&
        (prop.growth_ticks==0 || (!prop.broken && prop.hp==1));
}
