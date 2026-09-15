#include "counterweight.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../props/interaction.hpp"
#include <algorithm>
#include <vector>

namespace {
void reel(Entity& actor,int height,int progress) {
    actor.counter_a=height;actor.counter_b=progress;
    actor.label_a=WeightReel;actor.timer_a=120;
}
void slam(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    const Cell target=actor.point_b,source=actor.cell;
    const Handle owner{slot,actor.generation};
    std::vector<Handle> victims;
    // Capture before props open containers or a victim's death creates loot.
    for (int i=0;i<max_entities;++i) {
        const Entity& other=game.entities[static_cast<std::size_t>(i)];
        if (i!=slot && other.kind!=EntityKind::None && other.health>0 && other.cell==target)
            victims.push_back({i,other.generation});
    }
    reel(actor,0,30);
    hit_prop(game,target,120,source);
    for (Handle victim:victims) {
        const Entity* other=get_entity(game,victim);
        if (other && other->cell==target) damage_entity(game,victim.slot,48,source,false,owner);
    }
    emit_sound(game,SoundId::WeightSlam,target);
    emit_sound(game,SoundId::WeightReel,source);
}
}
// SLOTS: point_a anchored winch; point_b fixed impact cell; label_a phase;
// timer_a phase clock; counter_a rewind's starting height /48 and counter_b
// starting horizontal travel /30. The visible suspended mass has no extra hitbox.
void init_counterweight(Entity& actor) {
    actor.health=actor.max_health=140;actor.impassable=actor.hard_blocker=true;
    actor.sprite=Sprite::Counterweight;actor.point_a=actor.point_b=actor.cell;
}
void interrupt_counterweight(Entity& actor) {
    if (actor.kind!=EntityKind::Counterweight) return;
    if (actor.label_a==WeightTravel) reel(actor,48,std::clamp(30-actor.timer_a,0,30));
    else if (actor.label_a==WeightWarn) reel(actor,48,30);
    else if (actor.label_a==WeightDrop) {
        const int elapsed=std::clamp(8-actor.timer_a,0,8);
        reel(actor,48-elapsed*elapsed*48/64,30);
    }
}
void step_counterweight(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.cell!=actor.point_a) {
        actor.point_a=actor.point_b=actor.cell;reel(actor,48,0);return;
    }
    if (actor.freeze_ticks || actor.stun_ticks || actor.sleep_ticks || actor.toss.ticks) {
        interrupt_counterweight(actor);return;
    }
    if (actor.timer_a>0) return;
    switch (actor.label_a) {
    case WeightIdle: {
        const auto target=enemy_target(game,actor.cell,weight_reach);
        if (!target || target->cell==actor.cell || !clear_attack_sight(game,actor.cell,target->cell)) {
            actor.timer_a=18;return;
        }
        actor.point_b=target->cell;actor.label_a=WeightTravel;actor.timer_a=30;
        emit_sound(game,SoundId::WeightTravel,actor.cell);break;
    }
    case WeightTravel:
        actor.label_a=WeightWarn;actor.timer_a=48;
        emit_sound(game,SoundId::WeightWarn,actor.point_b);break;
    case WeightWarn:
        actor.label_a=WeightDrop;actor.timer_a=8;break;
    case WeightDrop: slam(game,slot);break;
    case WeightReel:
        actor.point_b=actor.cell;actor.label_a=WeightIdle;actor.counter_a=actor.counter_b=0;break;
    }
}
void break_counterweight(Game& game,const Entity& actor) {
    if (actor.kind!=EntityKind::Counterweight) return;
    Tile* tile=game.stage.at(actor.cell);
    // Keep the wreck in the old blocked footprint, never on a marked route or
    // under a fleeing victim. Existing props/contents are not overwritten.
    if (tile && tile->prop.kind==PropKind::None) place_prop(game.stage,actor.cell,PropKind::HoistWreck);
}
bool valid_counterweight(const Entity& actor) {
    if (actor.kind!=EntityKind::Counterweight) return true;
    if (actor.label_a<WeightIdle || actor.label_a>WeightReel || actor.timer_a<0 || actor.timer_a>120 ||
        actor.counter_a<0 || actor.counter_a>48 || actor.counter_b<0 || actor.counter_b>30 ||
        distance(actor.point_a,actor.point_b)>weight_reach) return false;
    constexpr int limits[]{18,30,48,8,120};
    return actor.timer_a<=limits[actor.label_a];
}
