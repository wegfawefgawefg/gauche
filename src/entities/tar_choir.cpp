#include "tar_choir.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../projectiles/tar_spit.hpp"
#include "../world/ground_items.hpp"

// SLOTS: entity_a immutable trio identity, including after its first body dies.
// counter_a voice/rank 0..2; counter_b committed singers' bit mask. label_a phase,
// label_b disrupted rhythm; timer_a phase clock. point_a stance, point_b aimed cell.
// c slots remain available for ordinary hearing memory. Singers sway in place.
namespace {
bool same_choir(const Entity& a,const Entity& b) {
    return b.kind==EntityKind::TarChoir && a.entity_a==b.entity_a;
}
bool silenced(const Entity& actor) {
    return actor.sleep_ticks>0 || actor.stun_ticks>0 || actor.freeze_ticks>0 ||
        actor.vitals.rooted>0 || actor.toss.ticks>0;
}
bool ready(const Entity& actor) {
    return actor.health>0 && actor.label_a==ChoirRest && actor.timer_a==0 && !silenced(actor);
}
bool lost_rhythm(const Game& game,const Entity& actor) {
    int heard=1<<actor.counter_a;
    for (const Entity& other:game.entities) {
        if (!same_choir(actor,other) || other.counter_a==actor.counter_a ||
            !(actor.counter_b & (1<<other.counter_a))) continue;
        if (other.health<=0 || silenced(other) || other.label_b!=0 || other.cell!=other.point_a ||
            distance(actor.cell,other.cell)>8 || !clear_attack_sight(game,actor.cell,other.cell)) return true;
        heard|=1<<other.counter_a;
    }
    return heard!=actor.counter_b;
}
void begin_phrase(Game& game,int slot,Cell target) {
    Entity& lead=game.entities[static_cast<std::size_t>(slot)];
    std::array<int,3> singers{slot,-1,-1};int count=1;
    for (int i=0;i<max_entities && count<3;++i) {
        const Entity& other=game.entities[static_cast<std::size_t>(i)];
        if (i==slot || !same_choir(lead,other) || !ready(other) || distance(other.cell,target)>8) continue;
        bool connected=true;
        for (int j=0;j<count;++j) {
            const Entity& partner=game.entities[static_cast<std::size_t>(singers[static_cast<std::size_t>(j)])];
            if (distance(other.cell,partner.cell)>8 || !clear_attack_sight(game,other.cell,partner.cell)) connected=false;
        }
        if (connected && clear_shot_sight(game,other.cell,target)) singers[static_cast<std::size_t>(count++)]=i;
    }
    int mask=0;
    for (int i=0;i<count;++i) mask|=1<<game.entities[static_cast<std::size_t>(singers[static_cast<std::size_t>(i)])].counter_a;
    const Cell facing=cardinal_toward(lead.cell,target,lead.facing);
    const Cell side{-facing.y,facing.x};
    for (int i=0;i<count;++i) {
        Entity& singer=game.entities[static_cast<std::size_t>(singers[static_cast<std::size_t>(i)])];
        const int offset=count==1 ? 0 : singer.counter_a-1;
        Cell landing=target+Cell{side.x*offset,side.y*offset};
        if (!walkable(game.stage.at_or_border(landing)) || distance(singer.cell,landing)>9 ||
            !clear_shot_sight(game,singer.cell,landing)) landing=target;
        singer.point_a=singer.cell;singer.point_b=landing;singer.counter_b=mask;
        singer.facing=cardinal_toward(singer.cell,landing,singer.facing);
        singer.label_a=ChoirHum;singer.label_b=0;singer.timer_a=48+singer.counter_a*12;
        emit_sound(game,singer.counter_a==0 ? SoundId::ChoirLow : singer.counter_a==1 ? SoundId::ChoirMid : SoundId::ChoirHigh,singer.cell);
    }
}
}
void init_tar_singer(Game& game,Entity& actor) {
    actor.health=actor.max_health=32;actor.impassable=true;actor.move_interval=20;
    actor.sprite=Sprite::TarSinger;actor.point_a=actor.cell;
    actor.entity_a={static_cast<int>(&actor-game.entities.data()),actor.generation};
}
Handle spawn_tar_choir(Game& game,const std::array<Cell,3>& cells) {
    int free=0;
    for (const Entity& actor:game.entities) free+=actor.kind==EntityKind::None;
    if (free<3) return {};
    for (std::size_t i=0;i<cells.size();++i) {
        if (!walkable(game.stage.at_or_border(cells[i])) || entity_at(game,cells[i],false)>=0) return {};
        for (std::size_t j=0;j<i;++j)
            if (cells[i]==cells[j] || distance(cells[i],cells[j])>8 || !clear_attack_sight(game,cells[i],cells[j])) return {};
    }
    Handle root{};
    for (std::size_t i=0;i<cells.size();++i) {
        const Handle member=spawn_entity(game,EntityKind::TarChoir,cells[i]);
        if (i==0) root=member;
        Entity& actor=*get_entity(game,member);actor.entity_a=root;actor.counter_a=static_cast<int>(i);
    }
    return root;
}
void interrupt_tar_singer(Entity& actor) {
    if (actor.kind!=EntityKind::TarChoir || actor.label_a==ChoirRest) return;
    actor.label_a=ChoirRest;actor.label_b=1;actor.timer_a=90+actor.counter_a*30;actor.counter_b=0;
}
void step_tar_singer(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (silenced(actor)) {interrupt_tar_singer(actor);return;}
    if (actor.label_a==ChoirRest) {
        if (actor.timer_a>0) return;
        actor.label_b=0;
        const auto target=enemy_target(game,actor.cell,8);
        if (target && target->cell!=actor.cell && clear_shot_sight(game,actor.cell,target->cell)) begin_phrase(game,slot,target->cell);
        return;
    }
    if (actor.cell!=actor.point_a || (actor.label_a==ChoirHum && lost_rhythm(game,actor))) {
        emit_sound(game,SoundId::ChoirBreak,actor.cell);interrupt_tar_singer(actor);return;
    }
    if (actor.timer_a>0) return;
    if (actor.label_a==ChoirHum) {
        if (launch_tar_spit(game,slot,actor.point_b)) emit_sound(game,SoundId::ChoirCough,actor.cell);
        actor.label_a=ChoirCough;actor.timer_a=12;
    } else {
        actor.label_a=ChoirRest;actor.timer_a=132-actor.counter_a*12;actor.counter_b=0;
    }
}
void drop_tar_choir(Game& game,const Entity& actor) {
    for (const Entity& other:game.entities)
        if (same_choir(actor,other) && other.health>0) return;
    const auto roll=random_u32(game)%100;
    if (roll<30) place_ground_item(game,actor.cell,ItemKind::TarFlask);
    // The following 15% solvent-rag slot awaits an actual useful item implementation.
}
bool valid_tar_singer(const Entity& actor) {
    return actor.kind!=EntityKind::TarChoir || (actor.entity_a.slot>=0 && actor.entity_a.generation>0 &&
        actor.label_a>=ChoirRest && actor.label_a<=ChoirCough && actor.label_b>=0 && actor.label_b<=1 &&
        actor.counter_a>=0 && actor.counter_a<=2 && actor.counter_b>=0 && actor.counter_b<=7 &&
        actor.timer_a<=192 && (actor.label_a!=ChoirHum || (actor.counter_b & (1<<actor.counter_a))));
}
