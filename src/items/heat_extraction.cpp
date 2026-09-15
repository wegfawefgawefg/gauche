#include "glow_slag.hpp"
#include "heat_siphon.hpp"
#include "flare.hpp"
#include "../props/candle.hpp"
#include "../props/stove.hpp"
#include "../surfaces/interaction.hpp"

#include <algorithm>

namespace {
template<class T> int take(T& fuel, int limit) {
    const int amount=std::min(static_cast<int>(fuel),limit);
    fuel=static_cast<T>(fuel-amount);
    return amount;
}
}

int extract_heat(Game& game, Cell cell, int limit) {
    Tile* tile=game.stage.at(cell);
    if (!tile || limit<=0) return 0;
    // ONE SOURCE: Prefer exposed fuel, then floor flame, then a burning body/item.
    // Cold coal, permanent torches, living embers and lava cannot be bottled.
    Prop& prop=tile->prop;
    if (prop_has_flame(prop)) {
        const int amount=take(prop.growth_ticks,limit);
        if (prop.growth_ticks==0) {
            prop.variant=static_cast<std::uint8_t>(prop.variant & ~1U);
            emit_sound(game,prop.kind==PropKind::Candle ? SoundId::CandleSpent : SoundId::StoveSpent,cell);
        }
        return amount;
    }
    if (tile->surface.fire_ticks>0) return take(tile->surface.fire_ticks,limit);
    for (Entity& actor : game.entities) {
        if (actor.kind==EntityKind::None || actor.cell!=cell || (actor.health<=0 && actor.kind!=EntityKind::GroundItem)) continue;
        if (actor.kind==EntityKind::Campfire && actor.fire_tramples<5 && limit>=siphon_portion) {
            ++actor.fire_tramples; actor.fire_dim_ticks=60;
            if (actor.fire_tramples==5) {
                actor.sprite=Sprite::CampfireAsh; actor.light={}; actor.self_light={};
                emit_sound(game,SoundId::FireOut,cell);
            }
            return siphon_portion;
        }
        if (burning_flare(actor)) {
            const int amount=take(actor.timer_a,limit);
            if (actor.timer_a==0) douse_flare(game,actor);
            return amount;
        }
        if (actor.kind==EntityKind::BoilerTank && actor.counter_b>0)
            return take(actor.counter_b,limit); // Pressure already built still has to vent.
        if (actor.burn_ticks>0) return take(actor.burn_ticks,limit);
        if (actor.scorch_ticks>0) return take(actor.scorch_ticks,limit);
        Item* item=actor.kind==EntityKind::GroundItem ? &actor.ground_item : actor.inventory.held();
        if (item && item->flame_ticks>0) return take(item->flame_ticks,limit);
        if (item && glowing_slag(*item)) {
            const int amount=take(item->loaded,limit);
            if (!item->loaded) emit_sound(game,SoundId::GlowCool,cell);
            return amount;
        }
    }
    return 0;
}
