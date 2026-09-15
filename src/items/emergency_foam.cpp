#include "emergency_foam.hpp"
#include "../projectiles/projectile.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/temperature.hpp"
#include <algorithm>
#include <array>

namespace {
constexpr RegionalItem can{"Emergency Foam",
    "Throw to 4. After landing, expands for 1s, douses a small cross and fills empty ground with 10s soft cover. Shots tear it; heat eats it. Won't bridge gaps.",
    Sprite::EmergencyFoam,{1,4,1,0,45,PatternEffect::Utility},
    ItemAction::Throw,18,2,true,0,0,0,0,0,SoundId::FoamThrow};

void expand(Game& game,int slot) {
    const Entity shot=game.entities[static_cast<std::size_t>(slot)];
    std::array<Cell,5> cells{};int count=0;
    // Capture reach before adding cover, so the first mound cannot seal the rest.
    // A stopped can wets the near face of cover, never the far side of a wall.
    const bool enclosed=projectile_blocked(game,shot.cell);
    for (Cell direction:{Cell{0,0},{1,0},{-1,0},{0,1},{0,-1}}) {
        if (enclosed && direction!=Cell{}) continue;
        const Cell cell=shot.cell+direction;
        const Tile* tile=game.stage.at(cell);
        if (!tile) continue;
        bool sealed=false;
        for (const Entity& actor:game.entities)
            if (actor.cell==cell && actor.impassable &&
                (actor.kind==EntityKind::Door || actor.kind==EntityKind::EncounterGate)) sealed=true;
        if (!sealed) cells[static_cast<std::size_t>(count++)]=cell;
    }
    remove_entity(game,{slot,shot.generation});
    emit_sound(game,SoundId::FoamExpand,shot.cell);
    for (int i=0;i<count;++i) {
        const Cell cell=cells[static_cast<std::size_t>(i)];
        quench_cell(game,cell,SoundId::FoamDouse);
        Tile& tile=*game.stage.at(cell);
        // No overwrites, entombed actors/loot, bridges or terrain conversion.
        if (!(buildable(tile.kind) || tile.kind==TileKind::Ruin || tile.kind==TileKind::Ice || tile.kind==TileKind::Bridge) ||
            entity_at(game,cell,false)>=0 ||
            (tile.prop.kind!=PropKind::None && !tile.prop.broken)) continue;
        tile.prop={PropKind::FoamCover,12,0,false,foam_life_ticks};
    }
}
}

const RegionalItem* emergency_foam_item(ItemKind kind) {
    return kind==ItemKind::EmergencyFoam ? &can : nullptr;
}

// SLOTS: counter_a remaining flight, attack_interval total; timer_b 8-tick beat;
// label_b 0 flying / 1 landed, timer_a sealed expansion clock only after landing.
// point_a launch, entity_a owner, ground_item exact one-can payload.
bool throw_emergency_foam(Game& game,int owner,Cell direction,int reach) {
    const Entity& user=game.entities[static_cast<std::size_t>(owner)];
    if (distance({},direction)!=1 || user.inventory.held()->kind!=ItemKind::EmergencyFoam) return false;
    Entity* shot=get_entity(game,spawn_entity(game,EntityKind::Projectile,user.cell));
    if (!shot) return false;
    shot->label_a=static_cast<int>(ProjectileKind::FoamCan);
    shot->ground_item=*user.inventory.held();shot->ground_item.count=1;
    shot->entity_a={owner,user.generation};shot->point_a=user.cell;shot->facing=direction;
    shot->counter_a=shot->attack_interval=std::clamp(reach,1,4);
    shot->timer_b=8;shot->sprite=Sprite::EmergencyFoam;
    return true;
}

void step_foam_can(Game& game,int slot) {
    Entity& shot=game.entities[static_cast<std::size_t>(slot)];
    if (shot.kind!=EntityKind::Projectile || shot.label_a!=static_cast<int>(ProjectileKind::FoamCan)) return;
    if (shot.label_b==1) {
        if (shot.timer_a==0) expand(game,slot);
        return;
    }
    if (shot.timer_b>0) return;
    const Cell next=shot.cell+shot.facing;
    if (projectile_blocked(game,next,shot.counter_a>1)) shot.counter_a=0;
    else {shot.cell=next;--shot.counter_a;}
    shot.timer_b=8;
    if (shot.counter_a==0) {
        shot.label_b=1;shot.timer_a=foam_expand_ticks;
        emit_sound(game,SoundId::FoamLand,shot.cell);
    }
}

bool live_foam(const Prop& prop) {
    return prop.kind==PropKind::FoamCover && !prop.broken && prop.hp>0;
}

void step_foam_cover(Game& game,Cell cell) {
    Tile* tile=game.stage.at(cell);
    if (!tile || !live_foam(tile->prop)) return;
    Prop& prop=tile->prop;
    if (prop.growth_ticks>0) --prop.growth_ticks;
    if (prop.growth_ticks==0) {hit_prop(game,cell,prop.hp,cell);return;}
    const auto phase=static_cast<std::uint64_t>(cell.x+cell.y);
    if ((game.tick+phase)%30!=0) return;
    // Nearby heat costs two HP per half-second, without becoming new fire fuel.
    bool heated=false;
    for (Cell side:{Cell{0,0},{1,0},{-1,0},{0,1},{0,-1}})
        if (hot_cell(game,cell+side)) {heated=true;break;}
    tile->surface.fire_ticks=0;
    if (heated) hit_prop(game,cell,2,cell);
}

bool valid_foam_can(const Entity& actor) {
    if (actor.kind!=EntityKind::Projectile || actor.label_a!=static_cast<int>(ProjectileKind::FoamCan)) return true;
    return actor.ground_item.kind==ItemKind::EmergencyFoam && actor.ground_item.count==1 &&
        actor.label_b>=0 && actor.label_b<=1 && actor.attack_interval>=1 && actor.attack_interval<=4 &&
        actor.counter_a>=0 && actor.counter_a<=actor.attack_interval &&
        actor.timer_a>=0 && actor.timer_a<=foam_expand_ticks && actor.timer_b>=0 && actor.timer_b<=8 &&
        (actor.label_b==1 ? actor.counter_a==0 : actor.counter_a>0 && actor.timer_a==0) &&
        distance({},actor.facing)==1;
}
