#include "catalog.hpp"
#include "eel_battery.hpp"
#include "quarry_tools.hpp"
#include "air_bladder.hpp"
#include "cold_flask.hpp"
#include "cold_remedies.hpp"
#include "heat_capsule.hpp"
#include "ice_footing.hpp"
#include "ice_projectiles.hpp"

const RegionalItem* regional_item(ItemKind kind) {
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
