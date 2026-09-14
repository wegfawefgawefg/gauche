#include "snow_globe.hpp"
#include "../surfaces/whiteout.hpp"

namespace {

constexpr RegionalItem globe{"Snow Globe", "Break beside you: 6s of snow cover. Blocks sight for all sides, never shots. No damage.",
    Sprite::SnowGlobe, {1,1,2,0,60,PatternEffect::Utility},
    ItemAction::Material,20,2,true,0,0,0,0,0,SoundId::GlobeBreak};

} // namespace

const RegionalItem* snow_globe_item(ItemKind kind) {
    return kind == ItemKind::SnowGlobe ? &globe : nullptr;
}

bool use_snow_globe(Game& game, int slot, Cell direction) {
    const Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const int radius = item_pattern(*user.inventory.held()).blast_radius;
    const Cell center = user.cell+direction;
    if (!raise_whiteout(game,center,radius,360)) return false;
    emit_sound(game,SoundId::WhiteoutRush,center);
    return true;
}
