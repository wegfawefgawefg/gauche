#include "catalog.hpp"
#include "candles.hpp"
#include "coal.hpp"
#include "kettle.hpp"
#include "brine.hpp"
#include "alarms.hpp"
#include "fishing_line.hpp"
#include "fish.hpp"
#include "snow_globe.hpp"
#include "muffling.hpp"
#include "optics.hpp"
#include "eel_battery.hpp"
#include "snow_tools.hpp"
#include "quarry_tools.hpp"
#include "air_bladder.hpp"
#include "cold_flask.hpp"
#include "cold_remedies.hpp"
#include "heat_capsule.hpp"
#include "ice_footing.hpp"
#include "ice_projectiles.hpp"

const RegionalItem* regional_item(ItemKind kind) {
    if (const RegionalItem* kettle = kettle_item(kind)) return kettle;
    if (const RegionalItem* coal = coal_item(kind)) return coal;
    if (const RegionalItem* candle = candle_supply(kind)) return candle;
    if (const RegionalItem* brine = brine_item(kind)) return brine;
    if (const RegionalItem* globe = snow_globe_item(kind)) return globe;
    if (const RegionalItem* fish = fish_item(kind)) return fish;
    if (const RegionalItem* fishing = fishing_line_item(kind)) return fishing;
    if (const RegionalItem* alarm = alarm_item(kind)) return alarm;
    if (const RegionalItem* felt = muffling_item(kind)) return felt;
    if (const RegionalItem* optic = optics_item(kind)) return optic;
    if (const RegionalItem* snow = snow_tool_item(kind)) return snow;
    if (const RegionalItem* battery = eel_battery_item(kind)) return battery;
    if (const RegionalItem* quarry = quarry_item(kind)) return quarry;
    if (const RegionalItem* footing = ice_footing_item(kind)) return footing;
    if (const RegionalItem* projectile = ice_projectile_item(kind)) return projectile;
    if (const RegionalItem* air = air_bladder_item(kind)) return air;
    if (const RegionalItem* cold = cold_flask_item(kind)) return cold;
    if (const RegionalItem* heat = heat_capsule_item(kind)) return heat;
    if (const RegionalItem* remedy = cold_remedy_item(kind)) return remedy;
    return forest_item(kind);
}
