#include "stove.hpp"
#include "candle.hpp"
#include "../surfaces/interaction.hpp"

#include <algorithm>

bool stove_lit(const Prop& prop) {
    return prop.kind == PropKind::Stove && !prop.broken && prop.hp > 0 &&
        prop.growth_ticks > 0 && (prop.variant & 1);
}

bool prop_has_flame(const Prop& prop) { return candle_lit(prop) || stove_lit(prop); }

bool light_stove(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (!tile || tile->prop.kind != PropKind::Stove || tile->prop.broken ||
        tile->prop.growth_ticks == 0 || stove_lit(tile->prop) || surface_wet(*tile)) return false;
    tile->prop.variant = 1;
    emit_sound(game,SoundId::StoveLight,cell);
    return true;
}

bool douse_stove(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (!tile || !stove_lit(tile->prop)) return false;
    tile->prop.variant = 0;
    emit_sound(game,SoundId::StoveOut,cell);
    return true;
}

bool feed_stove(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (!tile || tile->prop.kind != PropKind::Stove || tile->prop.broken ||
        tile->prop.growth_ticks >= stove_fuel_limit) return false;
    // FUEL: Coal adds twenty seconds; a cold stove still needs an ignition source.
    tile->prop.growth_ticks = static_cast<std::uint16_t>(std::min(stove_fuel_limit,
        static_cast<int>(tile->prop.growth_ticks)+1200));
    emit_sound(game,SoundId::CoalFeed,cell);
    return true;
}

void step_stove(Game& game, Cell cell) {
    Tile& tile = *game.stage.at(cell);
    if (surface_wet(tile)) douse_stove(game,cell);
    if (!stove_lit(tile.prop)) return;
    if (--tile.prop.growth_ticks == 0) {
        tile.prop.variant = 0;
        emit_sound(game,SoundId::StoveSpent,cell);
    }
}
