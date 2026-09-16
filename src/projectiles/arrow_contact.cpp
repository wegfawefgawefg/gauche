#include "arrow_contact.hpp"
#include "arrow_fire.hpp"
#include "flight_contacts.hpp"
#include "../combat/parry.hpp"
#include "../items/fire.hpp"

namespace {
bool live_arrow(const Entity& shot) {
    return shot.kind==EntityKind::Projectile && shot.health>0 &&
        shot.label_a==static_cast<int>(ProjectileKind::Arrow) && shot.timer_a>0;
}
bool hit_actor(Game& game,int shot_slot,int actor_slot) {
    Entity& shot=game.entities[static_cast<std::size_t>(shot_slot)];
    const Entity& actor=game.entities[static_cast<std::size_t>(actor_slot)];
    const Handle projectile{shot_slot,shot.generation},victim{actor_slot,actor.generation};
    if (shot_slot==actor_slot || actor.kind==EntityKind::None || !actor.impassable || actor.health<=0 ||
        shot.cell!=actor.cell || victim==shot.entity_a) return false;
    if (!first_flight_contact(game,projectile,victim)) return false;
    if (parry_ranged_hit(game,actor_slot,shot.facing)) {
        reflect_projectile(shot,actor,actor_slot);
        forget_flight_contacts(game,projectile);
        return true;
    }
    const Cell impact=shot.cell;
    const int health=actor.health;
    damage_entity(game,actor_slot,shot.counter_b,impact-shot.facing,true,shot.entity_a);
    if (burning_arrow(shot) && actor.health<health) {
        ignite_struck_actor(game,actor_slot);
        ignite_arrow_impact(game,shot,impact);
    }
    emit_sound(game,shot.ground_item.kind==ItemKind::RivetGun ? SoundId::RivetImpact : SoundId::ArrowImpact,impact);
    if (shot.label_b==0 || shot.health<=0) {remove_entity(game,projectile);return true;}
    return false;
}
}

bool arrow_cell_contact(Game& game,int shot_slot) {
    Entity& shot=game.entities[static_cast<std::size_t>(shot_slot)];
    if (!live_arrow(shot)) return false;
    arrow_flame_contact(game,shot,shot.cell);
    for (int actor_slot=0;actor_slot<max_entities;++actor_slot)
        if (hit_actor(game,shot_slot,actor_slot)) return true;
    return false;
}
void contact_arrows(Game& game,int actor_slot) {
    const Entity& actor=game.entities[static_cast<std::size_t>(actor_slot)];
    if (actor.kind==EntityKind::None || !actor.impassable || actor.health<=0) return;
    for (int shot_slot=0;shot_slot<max_entities && actor.health>0;++shot_slot) {
        Entity& shot=game.entities[static_cast<std::size_t>(shot_slot)];
        if (!live_arrow(shot) || shot.cell!=actor.cell) continue;
        arrow_flame_contact(game,shot,shot.cell);
        hit_actor(game,shot_slot,actor_slot);
    }
}
