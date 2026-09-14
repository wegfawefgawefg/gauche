#include "catalog.hpp"
#include "ice_footing.hpp"

const RegionalItem* regional_item(ItemKind kind) {
    if (const RegionalItem* footing = ice_footing_item(kind)) return footing;
    return forest_item(kind);
}
