#include "flare.hpp"
#include "../projectiles/projectile.hpp"
#include "../items/fire.hpp"
#include "../surfaces/interaction.hpp"
#include "../entities/steam_leech.hpp"
#include "../world/chasm.hpp"

namespace {
constexpr RegionalItem flare{"Signal Flare","Fire to 8; lands and burns red for 15s. Contact burns for 4 HP/s. Water puts it out; heat melts ice.",
    Sprite::SignalFlare,{1,8,0,0,45,PatternEffect::Utility,true},
    ItemAction::Throw,10,3,true,0,0,0,0,0,SoundId::FlareLaunch};
void finish(Game& game,Entity& shot,SoundId sound) {
    if (shot.health<=0) return;
    shot.health=0; shot.light={}; shot.self_light={0,0,0};
    shot.counter_a=shot.timer_a=0;
    emit_sound(game,sound,shot.cell);
}
void land(Game& game,Entity& shot) {
    shot.counter_a=0; shot.label_b=1; shot.timer_a=900;
    shot.sprite=Sprite::FlareBurning; shot.light={7,1350,{255,69,42}};
    emit_sound(game,SoundId::FlareLand,shot.cell);
}
}
const RegionalItem* flare_item(ItemKind kind) { return kind==ItemKind::SignalFlare ? &flare : nullptr; }

bool burning_flare(const Entity& actor) {
    return actor.kind==EntityKind::Projectile && actor.label_a==static_cast<int>(ProjectileKind::Flare) &&
        actor.health>0 && actor.label_b==1 && actor.timer_a>0;
}
bool douse_flare(Game& game,Entity& actor) {
    if (actor.kind!=EntityKind::Projectile || actor.label_a!=static_cast<int>(ProjectileKind::Flare) || actor.health<=0) return false;
    finish(game,actor,SoundId::FlareOut); return true;
}

// SLOTS: label_b flight/burning, counter_a reach, timer_b travel, timer_a flight cap/fuel.
// Fuel begins on landing. These physical timers run even if an effect silences the launcher.
bool launch_flare(Game& game,int owner,const Item& item,Cell direction) {
    const Entity& user=game.entities[static_cast<std::size_t>(owner)];
    Entity* shot=get_entity(game,spawn_entity(game,EntityKind::Projectile,user.cell));
    if (!shot) return false;
    shot->label_a=static_cast<int>(ProjectileKind::Flare);
    shot->entity_a={owner,user.generation}; shot->point_a=user.cell;
    shot->facing=direction; shot->counter_a=shot->attack_interval=item_pattern(item).maximum;
    shot->timer_b=4; shot->timer_a=shot->counter_a*4+4;
    shot->ground_item=item;shot->ground_item.count=1;
    shot->sprite=Sprite::SignalFlare;shot->light={2,400,{255,91,54}};
    return true;
}

void step_flare(Game& game,int slot) {
    Entity& shot=game.entities[static_cast<std::size_t>(slot)];
    if (shot.kind!=EntityKind::Projectile || shot.label_a!=static_cast<int>(ProjectileKind::Flare) || shot.health<=0) return;
    if (shot.label_b==0) {
        if (shot.timer_a==0) land(game,shot);
        else if (shot.timer_b>0) return;
        else {
            const Cell next=shot.cell+shot.facing;
            if (projectile_blocked(game,next)) land(game,shot);
            else {
                shot.cell=next;--shot.counter_a;shot.timer_b=4;
                // CONTACT: Bodies stop the flare on their cell; there is no instant distant hit.
                if (shot.counter_a==0 || entity_at(game,next,true)>=0) land(game,shot);
            }
        }
    }
    if (shot.label_b==0) return;
    if (chasm_contact(game,slot)) return;
    const Tile* tile=game.stage.at(shot.cell);
    if (!tile || surface_wet(*tile)) { douse_flare(game,shot); return; }
    if (shot.timer_a==0) { finish(game,shot,SoundId::FlareSpent);return; }
    if (shot.timer_a%90==0) emit_sound(game,SoundId::FlareHiss,shot.cell);
    if (leech_drains_cell(game,shot.cell)) return;
    ignite_surface(game,shot.cell);
    for (int victim=0;victim<max_entities;++victim) {
        if (victim==slot || game.entities[static_cast<std::size_t>(victim)].cell!=shot.cell) continue;
        ignite_struck_actor(game,victim);
    }
}

// SNAPSHOT: Shared projectile slots must stay within this tool's actual bounds.
bool valid_flare_state(const Entity& actor) {
    if (actor.kind!=EntityKind::Projectile || actor.label_a!=static_cast<int>(ProjectileKind::Flare)) return true;
    return actor.ground_item.kind==ItemKind::SignalFlare && actor.label_b>=0 && actor.label_b<=1 &&
        actor.counter_a>=0 && actor.counter_a<=12 && actor.timer_a>=0 && actor.timer_a<=900 &&
        actor.timer_b>=0 && actor.timer_b<=4 && distance({},actor.facing)==1 &&
        (actor.label_b==0 || actor.counter_a==0);
}
