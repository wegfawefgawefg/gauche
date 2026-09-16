#include "dog.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "foraging.hpp"
#include "dispatch.hpp"
#include "attacks.hpp"

// SLOTS: label_a hunt/windup/recovery; timer_a phase; point_a bite origin,
// point_b bite cell, facing committed direction. Food keeps its b slots.
void init_dog(Entity& dog) {
    dog.sprite=Sprite::DogBody;
    dog.health=dog.max_health=28;
    dog.move_interval=13;
    dog.attack_interval=dog_windup_ticks+dog_recovery_ticks;
    dog.impassable=true;
}

void interrupt_dog_bite(Entity& dog) {
    if (dog.kind!=EntityKind::Dog || dog.label_a!=DogWindup) return;
    dog.label_a=DogRecover;dog.timer_a=dog_recovery_ticks;
    dog.sprite=Sprite::DogBody;dog.use_flash=0;
}

bool valid_dog_bite(const Entity& dog) {
    return dog.kind!=EntityKind::Dog || (dog.label_a>=DogHunt && dog.label_a<=DogRecover &&
        dog.timer_a>=0 && dog.timer_a<=dog_recovery_ticks &&
        (dog.label_a!=DogWindup || (distance({},dog.facing)==1 && dog.point_a+dog.facing==dog.point_b)));
}

void step_dog(Game& game,int slot) {
    Entity& dog=game.entities[static_cast<std::size_t>(slot)];
    if (step_foraging(game,slot,dog.label_a!=DogHunt)) return;
    if (dog.label_a==DogWindup) {
        if (dog.cell!=dog.point_a || dog.stun_ticks>0 || dog.sleep_ticks>0 || dog.toss.ticks>0) {
            interrupt_dog_bite(dog);return;
        }
        if (dog.timer_a==0) {
            resolve_enemy_attack(game,slot,6,SoundId::DogSnap);
            dog.label_a=DogRecover;dog.timer_a=dog_recovery_ticks;dog.sprite=Sprite::DogBite;
        }
        return;
    }
    if (dog.label_a==DogRecover) {
        if (!dog.use_flash) dog.sprite=Sprite::DogBody;
        if (!dog.timer_a) dog.label_a=DogHunt;
        return;
    }
    if (step_hearing(game,slot)) return;
    const auto target=enemy_target(game,dog.cell,7);
    if (!target) {wander(game,slot);return;}
    if (distance(dog.cell,target->cell)==1 && clear_attack_sight(game,dog.cell,target->cell)) {
        dog.point_a=dog.cell;dog.point_b=target->cell;dog.facing=target->cell-dog.cell;
        dog.label_a=DogWindup;dog.timer_a=dog_windup_ticks;dog.sprite=Sprite::DogCrouch;
        emit_sound(game,SoundId::DogGrowl,dog.cell);return;
    }
    approach(game,slot,target->cell);
}
