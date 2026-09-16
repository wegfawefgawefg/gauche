#include "liquid_transfer.hpp"
#include "interaction.hpp"
#include "../entities/boiler_tank.hpp"
#include "../world/water.hpp"
#include <algorithm>

bool pumpable_liquid(LiquidKind kind) {
    return kind==LiquidKind::Tar || kind==LiquidKind::Water || kind==LiquidKind::Oil || kind==LiquidKind::Sap ||
        kind==LiquidKind::Honey || kind==LiquidKind::Rot || kind==LiquidKind::Brine || kind==LiquidKind::Coolant;
}
bool pumpable_spill(const Tile& tile) {
    return walkable(tile.kind) && tile.kind!=TileKind::Lava && tile.kind!=TileKind::Ice &&
        tile.kind!=TileKind::Water && !river_water(tile.kind) && !tile.freeze_ticks &&
        !tile.surface.fire_ticks && tile.surface.liquid_ticks>0 && pumpable_liquid(tile.surface.liquid);
}
LiquidLoad collect_spill(Game& game,Cell cell,LiquidKind accepted,int capacity) {
    Tile* tile=game.stage.at(cell);
    if (!tile || capacity<=0 || !pumpable_spill(*tile)) return {};
    Surface& surface=tile->surface;
    if (accepted!=LiquidKind::None && accepted!=surface.liquid) return {};
    const LiquidLoad collected{surface.liquid,std::min(capacity,static_cast<int>(surface.liquid_ticks))};
    surface.liquid_ticks=static_cast<std::uint16_t>(surface.liquid_ticks-collected.amount);
    if (surface.liquid_ticks==0) surface.liquid=LiquidKind::None;
    return collected;
}
void splash_coolant(Game& game,Cell cell,int amount) {
    if (amount<=0) return;
    for (Entity& actor:game.entities) {
        if (actor.kind==EntityKind::None || actor.cell!=cell || actor.health<=0) continue;
        apply_chill(actor,std::min(180,amount));
        if (actor.kind==EntityKind::BoilerTank) reduce_boiler_pressure(actor,std::min(80,(amount*80+599)/600));
    }
}
int discharge_spill(Game& game,Cell cell,LiquidLoad load) {
    Tile* tile=game.stage.at(cell);
    if (!tile || load.amount<=0 || !pumpable_liquid(load.kind) || !walkable(tile->kind)) return 0;
    const Surface& surface=tile->surface;
    const bool same=surface.liquid==load.kind && surface.liquid_ticks>0;
    // No mixed tank or free replacement fuel. Water may wash a different spill
    // away, using the ordinary dousing rules; unlike liquids otherwise refuse.
    if (surface.liquid_ticks>0 && !same && !water_liquid(load.kind)) return 0;
    const int existing=same ? surface.liquid_ticks : 0;
    const int amount=std::min(load.amount,3600-existing);
    if (amount<=0 || !pour_surface(game,cell,load.kind,existing+amount)) return 0;
    if (load.kind==LiquidKind::Coolant) splash_coolant(game,cell,amount);
    return amount;
}
