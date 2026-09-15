#include "../game.hpp"
#include "pocket_door.hpp"
#include "../world/water.hpp"
#include "../surfaces/interaction.hpp"
#include "../props/interaction.hpp"
#include "../traps/woodland.hpp"

#include <algorithm>

void enter_actor_cell(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    if (actor.toss.ticks>0 || actor.health <= 0 || actor.move_interval == 0 || actor.hard_blocker ||
        actor.kind == EntityKind::Train) return;
    contact_surface(game, slot);
    if (!wading_actor(actor)) return; // Airborne actors do not stomp props or campfires.
    step_on_prop(game, slot);
    if (actor.health <= 0) return;

    const Cell arrival = actor.cell;
    enter_woodland_traps(game, slot);
    if (actor.toss.ticks>0 || actor.health <= 0 || actor.cell != arrival) return;

    // TRAMPLE: Only a successful step or shove calls this; standing still spends no fire life.
    for (Entity& fire : game.entities) {
        if (fire.kind != EntityKind::Campfire || fire.cell != actor.cell ||
            fire.fire_tramples >= 5 || actor.kind == EntityKind::SteamLeech || actor.kind == EntityKind::Ember) continue;
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
    if (actor.health > 0) enter_pocket_door(game, slot);
}
