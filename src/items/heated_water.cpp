#include "heated_water.hpp"
#include "../surfaces/temperature.hpp"
#include "../world/water.hpp"
#include <algorithm>

bool heated_water_item(ItemKind kind) {return kind==ItemKind::SteamKettle || kind==ItemKind::SteamLance;}
bool fresh_water(const Game& game,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    if (!tile) return false;
    const Surface& surface=tile->surface;
    if (surface.liquid_ticks>0 && surface.liquid!=LiquidKind::None && surface.liquid!=LiquidKind::Water) return false;
    return shallow_water(tile->kind) || tile->kind==TileKind::Water ||
        (surface.liquid==LiquidKind::Water && surface.liquid_ticks>0);
}
// loaded: empty/cold/hot (0/1/2). spare: heating progress or remaining heat.
// Inventory, loose cargo and containers use the same clock; swapping cannot refresh it.
void step_heated_water(Game& game,Item& item,Cell cell,bool wet) {
    if (!heated_water_item(item.kind) || item.loaded==0) return;
    if (wet) {item.loaded=1;item.spare=0;return;}
    const bool heated=warm_cell(game,cell),lance=item.kind==ItemKind::SteamLance;
    if (item.loaded==2) {
        if (heated) item.spare=water_cool_ticks;
        else if (--item.spare<=0) {
            item.loaded=1;item.spare=0;
            emit_sound(game,lance ? SoundId::LanceCool : SoundId::KettleCool,cell);
        }
        return;
    }
    item.spare=heated ? item.spare+1 : std::max(0,item.spare-1);
    if (item.spare<water_heat_ticks) return;
    item.loaded=2;item.spare=water_cool_ticks;
    emit_sound(game,lance ? SoundId::LanceReady : SoundId::KettleReady,cell);
}
