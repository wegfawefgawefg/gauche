#include "exposed_fuse.hpp"
#include "projectile.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

bool exposed_fuse(const Entity& actor) {
    if (actor.kind!=EntityKind::Projectile) return false;
    switch (static_cast<ProjectileKind>(actor.label_a)) {
    case ProjectileKind::Bomb: return actor.ground_item.kind==ItemKind::Bomb;
    case ProjectileKind::Firecracker: return actor.ground_item.kind==ItemKind::Firecracker;
    case ProjectileKind::Mixture: return actor.ground_item.kind==ItemKind::PitchBomb;
    case ProjectileKind::QuarryCharge: return actor.ground_item.kind==ItemKind::QuarryCharge;
    default: return false; // Prism and thaw charges have sealed internal reactions.
    }
}
bool quench_exposed_fuse(Entity& actor,bool cold) {
    if (!exposed_fuse(actor) || actor.timer_a<=0) return false;
    if (cold) {
        const bool changed=actor.freeze_ticks<120;
        actor.freeze_ticks=std::max(actor.freeze_ticks,120);
        return changed;
    }
    // Keep the actual item, cell and generation; conversion cannot fail at entity capacity.
    const auto generation=actor.generation; const auto born=actor.birth_tick;
    const Cell cell=actor.cell; const Item item=actor.ground_item;
    actor={}; actor.kind=EntityKind::GroundItem; actor.generation=generation; actor.birth_tick=born;
    actor.cell=cell; actor.ground_item=item; actor.sprite=item_sprite(item);
    actor.health=actor.max_health=1; actor.light=item.light;
    return true;
}
void wet_landed_fuse(Game& game,int slot) {
    auto& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.counter_a==0 && exposed_fuse(actor) && surface_wet(game.stage.at_or_border(actor.cell)) &&
        quench_exposed_fuse(actor,false)) emit_sound(game,SoundId::FuseDouse,actor.cell);
}
