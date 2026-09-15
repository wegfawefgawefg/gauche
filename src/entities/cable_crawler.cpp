#include "cable_crawler.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/conduction.hpp"
#include <algorithm>
#include <array>

namespace {
constexpr std::array<Cell,4> sides{{{1,0},{0,1},{-1,0},{0,-1}}};
void rest(Entity& actor) {
    actor.label_a=CrawlerRecover;actor.timer_a=90;actor.sprite=Sprite::CableCrawler;
    actor.self_light={};actor.light={};
}
void lay_wire(Game& game,Entity& actor) {
    Tile* tile=game.stage.at(actor.cell);
    if (!tile || actor.counter_a==0 || !walkable(*tile) || tile->kind==TileKind::Lava ||
        tile->prop.kind!=PropKind::None) return;
    if (place_prop(game.stage,actor.cell,PropKind::CopperWire)) --actor.counter_a;
}
bool wall_edge(const Game& game,Cell cell) {
    return std::any_of(sides.begin(),sides.end(),[&](Cell side){
        return !walkable(game.stage.at_or_border(cell+side).kind);
    });
}
void crawl(Game& game,int slot,const EnemyTarget* target) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.move_wait>0 || actor.vitals.rooted>0) return;
    int best=-10000;Cell next=actor.cell;
    // Prefer adjacent walls and continuing forward, but score all four exits.
    // Target distance can pull it away from a wall; no fixed-direction deadlock.
    for (Cell direction:sides) {
        const Cell cell=actor.cell+direction;const Tile* tile=game.stage.at(cell);
        if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || entity_at(game,cell,true)>=0) continue;
        int score=wall_edge(game,cell) ? 3 : 0;
        if (direction==actor.facing) score+=2;
        if (direction==Cell{-actor.facing.x,-actor.facing.y}) score-=2;
        if (target) score-=distance(cell,target->cell)*3;
        if (score>best) {best=score;next=cell;}
    }
    if (next==actor.cell) {actor.move_wait=12;return;}
    if (willing_step(game,slot,next) && actor.kind==EntityKind::CableCrawler && actor.health>0) {
        lay_wire(game,actor);
        if (actor.timer_b==0) {emit_sound(game,SoundId::CrawlerStep,actor.cell);actor.timer_b=36;}
    }
}
}
// SLOTS: label_a phase; timer_a charge/recovery; counter_a remaining wire cells
// (eight lifetime, existing wire costs nothing); timer_b local foot cue throttle.
// point_a committed origin, point_b electrode contact. c slots remain hearing.
void init_cable_crawler(Entity& actor) {
    actor.health=actor.max_health=50;actor.move_interval=20;actor.impassable=true;
    actor.counter_a=8;actor.sprite=Sprite::CableCrawler;
}
void interrupt_cable_crawler(Entity& actor) {
    if (actor.kind==EntityKind::CableCrawler && actor.label_a==CrawlerCharge) rest(actor);
}
void step_cable_crawler(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.health<=0) return;
    if (actor.freeze_ticks>0 || actor.vitals.rooted>0) interrupt_cable_crawler(actor);
    if (actor.label_a==CrawlerRecover) {
        if (actor.timer_a==0) actor.label_a=CrawlerRoam;
        return;
    }
    if (actor.label_a==CrawlerCharge) {
        if (actor.cell!=actor.point_a) {rest(actor);return;}
        if (actor.timer_a>0) return;
        if (!clear_attack_sight(game,actor.cell,actor.point_b,false)) {rest(actor);return;}
        auto shock=trace_conducted_shock(game,actor.point_b,6);
        // The electrode is insulated from its own chassis. Other electricity
        // still hurts this machine; other crawlers and allies can be hit here.
        const Handle self{slot,actor.generation};
        std::erase_if(shock.victims,[&](const ShockVictim& victim){return victim.handle==self;});
        const Cell origin=actor.cell;
        rest(actor); // Apply before damage: chain reactions cannot repeat this pulse.
        if (shock.wave.ground_node<0 && live_decoy(game.stage.at_or_border(actor.point_b).prop))
            hit_prop(game,actor.point_b,14,origin);
        apply_conducted_shock(game,shock,origin,14);
        emit_sound(game,SoundId::CrawlerPulse,origin);return;
    }
    lay_wire(game,actor);
    const auto target=enemy_target(game,actor.cell,8);
    if (target && actor.freeze_ticks==0 && actor.vitals.rooted==0) {
        Cell contact=actor.cell;bool ready=false;
        if (distance(actor.cell,target->cell)==1 && clear_attack_sight(game,actor.cell,target->cell)) {
            contact=target->cell;ready=true;
        } else {
            const auto shock=trace_conducted_shock(game,actor.cell,6);
            // A ground sink prevents victim enumeration, but an approaching
            // crawler still discharges into it when the target shares the wire.
            for (int i=0;i<shock.wave.count;++i)
                if (shock.wave.nodes[static_cast<std::size_t>(i)].cell==target->cell &&
                    get_entity(game,target->actor) && water_shock_target(game,*get_entity(game,target->actor))) ready=true;
        }
        if (ready) {
            actor.point_a=actor.cell;actor.point_b=contact;
            actor.facing=cardinal_toward(actor.cell,target->cell,actor.facing);
            actor.label_a=CrawlerCharge;actor.timer_a=48;actor.sprite=Sprite::CrawlerCharge;
            actor.light={2,400,{105,213,232}};
            emit_sound(game,SoundId::CrawlerCharge,actor.cell);return;
        }
    }
    crawl(game,slot,target ? &*target : nullptr);
}
bool valid_cable_crawler(const Entity& actor) {
    if (actor.kind!=EntityKind::CableCrawler) return true;
    if (actor.label_a<CrawlerRoam || actor.label_a>CrawlerRecover || actor.timer_a<0 || actor.timer_a>90 ||
        actor.counter_a<0 || actor.counter_a>8 || actor.timer_b<0 || actor.timer_b>36) return false;
    return actor.label_a!=CrawlerCharge || (actor.timer_a<=48 && distance(actor.point_a,actor.point_b)<=1);
}
