#include "arrow_fire.hpp"
#include "../surfaces/temperature.hpp"
#include "../surfaces/interaction.hpp"

// Arrow counter_c is a saved flame flag. It does not burn the 1-HP projectile
// as a creature, change its weapon payload, or extend its finite flight life.
bool wooden_arrow(const Entity& shot) {
    return shot.kind==EntityKind::Projectile && shot.label_a==static_cast<int>(ProjectileKind::Arrow) &&
        (shot.ground_item.kind==ItemKind::Bow || shot.ground_item.kind==ItemKind::Crossbow);
}

bool burning_arrow(const Entity& shot) { return wooden_arrow(shot) && shot.counter_c==1; }

void arrow_flame_contact(Game& game,Entity& shot,Cell cell) {
    if (!wooden_arrow(shot) || burning_arrow(shot) || !flame_cell(game,cell)) return;
    shot.counter_c=1;
    shot.light={2,380,{255,142,49}};
    emit_sound(game,SoundId::FireCatch,cell);
}

bool douse_arrow(Entity& shot) {
    if (!burning_arrow(shot)) return false;
    shot.counter_c=0;shot.light={};
    return true;
}

void ignite_arrow_impact(Game& game,const Entity& shot,Cell cell) {
    if (burning_arrow(shot)) ignite_surface(game,cell);
}
