#include "fire.hpp"
#include "kettle.hpp"
#include "storm_lantern.hpp"
#include "../surfaces/interaction.hpp"
#include "../world/water.hpp"

#include <algorithm>

LightEmitter item_light(const Item& item) {
    if (item.kind == ItemKind::StormLantern) return lantern_light(item);
    return item.flame_ticks > 0 ? LightEmitter{4, 950, {255, 133, 43}} : item.light;
}

bool light_stick(Game& game, Item& item, Cell source) {
    if (item.kind != ItemKind::Stick || item.flame_ticks > 0) return false;
    item.flame_ticks = stick_flame_duration;
    emit_sound(game, SoundId::FireCatch, source);
    return true;
}

// LIFETIME: Stowing, dropping and swapping do not refresh a flame. Burnout leaves
// the remaining stick condition intact; water puts it out early.
void step_item_state(Game& game, Item& item, Cell cell, bool wet) {
    item.cooldown = std::max(0, item.cooldown - 1);
    step_kettle(game,item,cell,wet);
    if (item.flame_ticks <= 0) return;
    --item.flame_ticks;
    if (wet) {
        item.flame_ticks = 0;
        emit_sound(game, SoundId::WaterDouse, cell);
    }
}

void ignite_struck_actor(Game& game, int slot) {
    Entity& target = game.entities[static_cast<std::size_t>(slot)];
    if (target.health <= 0 || (target.move_interval <= 0 && target.kind != EntityKind::RootTurret && target.kind != EntityKind::WaspNest) ||
        target.kind == EntityKind::SlagSnail || target.kind == EntityKind::FurnaceMoth || target.kind == EntityKind::WalkingKiln || target.kind == EntityKind::Ember || target.kind == EntityKind::SteamLeech || target.kind == EntityKind::Train ||
        target.kind == EntityKind::RailLayer) return;
    if ((wading_actor(target) || target.kind == EntityKind::RootTurret || target.kind == EntityKind::WaspNest) && surface_wet(game.stage.at_or_border(target.cell))) return;
    if (target.scorch_ticks == 0) emit_sound(game, SoundId::FirePanic, target.cell);
    target.scorch_ticks = std::max(target.scorch_ticks, 300);
}
