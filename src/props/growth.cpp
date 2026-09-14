#include "growth.hpp"
#include "ice_cover.hpp"
#include "alarm_clock.hpp"
#include "../surfaces/interaction.hpp"

namespace {

bool plantable(const Tile& tile) {
    return buildable(tile.kind) && !surface_wet(tile) && tile.surface.fire_ticks == 0;
}

} // namespace

// PLANTING: Don't bury loot, fixtures or another body under a new prop.
bool plant_prop(Game& game, Cell cell, PropKind kind) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr || !plantable(*tile) || entity_at(game, cell, false) >= 0 ||
        (tile->prop.kind != PropKind::None && !tile->prop.broken)) return false;
    tile->prop = {};
    if (!place_prop(game.stage, cell, kind)) return false;
    if (kind == PropKind::Shoot) tile->prop.growth_ticks = 180;
    return true;
}

// GROWTH: A body or dropped item can hold a shoot down until the cell is clear.
void step_prop_growth(Game& game) {
    for (int y = 0; y < game.stage.height; ++y)
        for (int x = 0; x < game.stage.width; ++x) {
            const Cell cell{x, y};
            Tile& tile = *game.stage.at(cell);
            Prop& prop = tile.prop;
            if (prop.kind == PropKind::AlarmClock) { step_alarm_clock(game, cell); continue; }
            if (prop.kind == PropKind::IceBlock) { age_ice_cover(game, cell); continue; }
            if (prop.kind != PropKind::Shoot || prop.broken) continue;
            if (prop.growth_ticks > 1) { --prop.growth_ticks; continue; }
            if (!plantable(tile) || entity_at(game, cell, false) >= 0) continue;
            const int damage = prop_spec(prop.kind).health - prop.hp;
            prop.kind = PropKind::RootCover;
            // Damage to the shoot is retained when it matures.
            prop.hp = static_cast<std::uint8_t>(prop_spec(prop.kind).health - damage);
            prop.growth_ticks = 0;
            emit_sound(game, SoundId::CoverGrow, cell);
        }
}

LightEmitter prop_light(const Prop& prop) {
    return prop.broken || prop.covered ? LightEmitter{} : prop_spec(prop.kind).light;
}
