#include "audit_clerk.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "mine_crew.hpp"
#include "../props/pay_cage.hpp"
#include <algorithm>

namespace {
void rest(Entity& actor) {
    actor.label_a=ClerkRest;actor.timer_a=30;actor.entity_a={};actor.sprite=Sprite::AuditClerk;
}
bool walk_to(Game& game,int slot,Cell target) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.cell==target) return true;
    if (actor.move_wait>0) return false;
    if (const auto next=next_route_cell(game,slot,target,512)) willing_step(game,slot,*next);
    else actor.move_wait=18;
    return false;
}
std::optional<Cell> cage_stance(const Game& game,int slot,Cell cage) {
    const Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    const Tile* tile=game.stage.at(cage);
    if (!tile || !live_pay_cage(tile->prop) || tile->prop.growth_ticks>=60000) return {};
    if (distance(actor.cell,cage)==1) return actor.cell;
    for (Cell side:{Cell{1,0},{0,1},{-1,0},{0,-1}}) {
        const Cell stand=cage+side;const Tile* floor=game.stage.at(stand);
        if (!floor || !walkable(*floor) || floor->kind==TileKind::Lava || entity_at(game,stand,true)>=0) continue;
        if (next_route_cell(game,slot,stand,512)) return stand;
    }
    return {};
}
void find_money(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    int nearest=10;Handle found;
    for (int i=0;i<max_entities;++i) {
        const Entity& coin=game.entities[static_cast<std::size_t>(i)];
        if (coin.kind!=EntityKind::Coins || coin.counter_a<=0) continue;
        const int gap=distance(actor.cell,coin.cell);
        const Tile* tile=game.stage.at(coin.cell);
        if (gap>=nearest || !tile || !walkable(*tile) || tile->kind==TileKind::Lava) continue;
        const int occupant=entity_at(game,coin.cell,true);
        if (occupant>=0 && occupant!=slot) continue;
        if (gap>0 && !next_route_cell(game,slot,coin.cell,512)) continue;
        found={i,coin.generation};nearest=gap;
    }
    actor.entity_a=found;actor.timer_b=30;
}
void alarm(Game& game,int slot,Cell source) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.kind!=EntityKind::AuditClerk || actor.health<=0) return;
    const int culprit=entity_at(game,source,true);
    if (culprit==slot || (culprit>=0 && game.entities[static_cast<std::size_t>(culprit)].kind==EntityKind::AuditClerk)) {rest(actor);return;}
    actor.entity_b=culprit>=0 ? Handle{culprit,game.entities[static_cast<std::size_t>(culprit)].generation} : Handle{};
    if (actor.label_a==ClerkAlarm) return; // Repeated hits cannot restart the warning clock.
    actor.point_b=actor.cell;actor.entity_a={};
    if (actor.attack_wait>0) {actor.label_a=ClerkFlee;actor.timer_a=180;return;}
    actor.label_a=ClerkAlarm;actor.timer_a=36;actor.sprite=Sprite::ClerkAlarm;
    emit_sound(game,SoundId::ClerkGasp,actor.cell);
}
}
// SLOTS: label_a phase; counter_a actual carried gold (0..20); point_a pay cage;
// point_b stamp/deposit/alarm origin; entity_a coin handle; entity_b culprit;
// timer_a phase, timer_b coin-search throttle, attack_wait alarm cooldown.
// The c slots remain shared hearing. No invented coin balance lives in a sprite.
void init_audit_clerk(Entity& actor) {
    actor.health=actor.max_health=38;actor.move_interval=16;actor.impassable=true;
    actor.point_a=actor.cell;actor.sprite=Sprite::AuditClerk;
}
void hurt_audit_clerk(Game& game,int slot,Cell source) { alarm(game,slot,source); }
void alarm_pay_clerks(Game& game,Cell cage,Cell source) {
    const auto heard=audible_cells(game,cage,8);
    for (int slot=0;slot<max_entities;++slot) {
        const Entity& actor=game.entities[static_cast<std::size_t>(slot)];
        if (actor.kind==EntityKind::AuditClerk && actor.point_a==cage &&
            std::find(heard.begin(),heard.end(),actor.cell)!=heard.end()) alarm(game,slot,source);
    }
}
void interrupt_audit_clerk(Entity& actor) {
    if (actor.kind==EntityKind::AuditClerk && (actor.label_a==ClerkStamp ||
        actor.label_a==ClerkDeposit || actor.label_a==ClerkAlarm)) rest(actor);
}
void step_audit_clerk(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.health<=0) return;
    if (actor.vitals.rooted>0 || actor.freeze_ticks>0) interrupt_audit_clerk(actor);
    if (actor.label_a==ClerkRest) {
        if (actor.timer_a==0) actor.label_a=actor.counter_a>0 ? ClerkReturn : ClerkSeek;
        return;
    }
    if (actor.label_a==ClerkFlee) {
        if (actor.timer_a==0) {rest(actor);return;}
        const Entity* threat=get_entity(game,actor.entity_b);
        if (threat && threat->health>0) flee(game,slot,threat->cell);else wander(game,slot);
        return;
    }
    if (actor.label_a==ClerkAlarm || actor.label_a==ClerkStamp || actor.label_a==ClerkDeposit) {
        if (actor.cell!=actor.point_b) {rest(actor);return;}
        if (actor.timer_a>0) return;
        if (actor.label_a==ClerkAlarm) {
            alarm_mine_workers(game,actor.cell,actor.entity_b);
            emit_sound(game,SoundId::ClerkAlarm,actor.cell);actor.attack_wait=300;
            actor.label_a=ClerkFlee;actor.timer_a=180;actor.sprite=Sprite::AuditClerk;return;
        }
        if (actor.label_a==ClerkStamp) {
            Entity* coin=get_entity(game,actor.entity_a);
            if (coin && coin->kind==EntityKind::Coins && coin->counter_a>0 && coin->cell==actor.cell && actor.counter_a==0) {
                actor.counter_a=std::min(20,coin->counter_a);coin->counter_a-=actor.counter_a;
                if (coin->counter_a==0) remove_entity(game,actor.entity_a);
                emit_sound(game,SoundId::ClerkStamp,actor.cell);
            }
        } else {
            Tile* tile=game.stage.at(actor.point_a);
            if (tile && live_pay_cage(tile->prop) && distance(actor.cell,actor.point_a)==1) {
                const int amount=std::min(actor.counter_a,60000-static_cast<int>(tile->prop.growth_ticks));
                tile->prop.growth_ticks=static_cast<std::uint16_t>(tile->prop.growth_ticks+amount);
                actor.counter_a-=amount;
                if (amount>0) emit_sound(game,SoundId::PayDeposit,actor.point_a);
            }
        }
        rest(actor);return;
    }
    if (actor.counter_a>0) {
        actor.label_a=ClerkReturn;
        if (actor.move_wait>0) return;
        if (const auto stand=cage_stance(game,slot,actor.point_a)) {
            if (walk_to(game,slot,*stand)) {
                actor.label_a=ClerkDeposit;actor.point_b=actor.cell;actor.timer_a=30;
                actor.facing=cardinal_toward(actor.cell,actor.point_a,actor.facing);actor.sprite=Sprite::ClerkStamp;
            }
        } else wander(game,slot); // Broken/blocked cage leaves money on the carrier.
        return;
    }
    if (actor.timer_b==0 || !get_entity(game,actor.entity_a)) {
        if (actor.timer_b>0 && actor.entity_a.slot<0) {wander(game,slot);return;}
        find_money(game,slot);
    }
    const Entity* coin=get_entity(game,actor.entity_a);
    if (!coin || coin->kind!=EntityKind::Coins || coin->counter_a<=0) {actor.entity_a={};return;}
    if (walk_to(game,slot,coin->cell)) {
        actor.label_a=ClerkStamp;actor.timer_a=30;actor.point_b=actor.cell;actor.sprite=Sprite::ClerkStamp;
        emit_sound(game,SoundId::ClerkPaper,actor.cell);
    }
}
bool valid_audit_clerk(const Entity& actor) {
    return actor.kind!=EntityKind::AuditClerk || (actor.label_a>=ClerkSeek && actor.label_a<=ClerkRest &&
        actor.counter_a>=0 && actor.counter_a<=20 && actor.timer_a>=0 && actor.timer_a<=180 &&
        actor.timer_b>=0 && actor.timer_b<=30 && actor.attack_wait>=0 && actor.attack_wait<=300);
}
