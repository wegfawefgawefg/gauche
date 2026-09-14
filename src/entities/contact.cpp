#include "../game.hpp"
#include "../props/interaction.hpp"

#include <algorithm>

void enter_actor_cell(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    if (actor.health <= 0 || actor.move_interval == 0 || actor.hard_blocker ||
        actor.kind == EntityKind::Ember || actor.kind == EntityKind::Train) return;
    step_on_prop(game, slot);

    // TRAMPLE: Only a successful step or shove calls this; standing still spends no fire life.
    for (Entity& fire : game.entities) {
        if (fire.kind != EntityKind::Campfire || fire.cell != actor.cell ||
            fire.fire_tramples >= 5) continue;
        if (actor.scorch_ticks == 0) emit_sound(game, SoundId::FirePanic, actor.cell);
        // REIGNITION: Extend the same weak burn, never stack its damage rate.
        actor.scorch_ticks = std::max(actor.scorch_ticks, 300);
        ++fire.fire_tramples;
        fire.fire_dim_ticks = 60;
        emit_sound(game, SoundId::FireTrample, fire.cell);
        if (fire.fire_tramples == 5) {
            fire.sprite = Sprite::CampfireAsh;
            fire.light = {};
            fire.self_light = {0, 0, 0};
            emit_sound(game, SoundId::FireOut, fire.cell);
        }
    }
}
