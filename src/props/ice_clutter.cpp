#include "ice_clutter.hpp"
#include "interaction.hpp"
#include "../surfaces/temperature.hpp"
#include "../surfaces/interaction.hpp"

void step_ice_clutter(Game& game,Cell cell) {
    auto& tile=*game.stage.at(cell);
    if (tile.prop.broken || game.tick%30!=0 || !warm_cell(game,cell)) return;
    hit_prop(game,cell,tile.prop.hp,cell);
    if (tile.surface.liquid==LiquidKind::None || tile.surface.liquid==LiquidKind::Water)
        pour_surface(game,cell,LiquidKind::Water,120);
    emit_sound(game,SoundId::RoofMelt,cell);
}
