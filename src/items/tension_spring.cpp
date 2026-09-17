#include "tension_spring.hpp"
#include "../props/interaction.hpp"
#include "../props/tension_spring.hpp"
#include "../world/water.hpp"
#include "../surfaces/interaction.hpp"

namespace {
constexpr RegionalItem spring{"Tension Spring",
    "Set underfoot, aimed. Arms in 0.3s, then launches the next walker four tiles. Cross gaps or fling pursuers; walls hurt. Single-use. Heat ruins it.",
    Sprite::TensionSpring,{0,0,0,0,36,PatternEffect::Utility},
    ItemAction::Material,15,3,true,0,0,0,0,0,SoundId::TensionSet};
}
const RegionalItem* tension_spring_item(ItemKind kind) { return kind==ItemKind::TensionSpring ? &spring : nullptr; }
bool place_tension_spring(Game& game,int slot,Cell direction) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Tile* tile=game.stage.at(user.cell);
    if (user.inventory.held()->kind!=ItemKind::TensionSpring || !wading_actor(user) || user.toss.ticks>0 ||
        !tile || !walkable(*tile) || tile->kind==TileKind::Lava || tile->kind==TileKind::Rail ||
        tile->surface.fire_ticks>0 || (tile->prop.kind!=PropKind::None && !tile->prop.broken) ||
        distance({},direction)!=1) return false;
    for (const Entity& other:game.entities)
        if (other.kind!=EntityKind::None && other.cell==user.cell && other.hard_blocker) return false;
    int facing=0;
    while (tension_direction(static_cast<std::uint8_t>(facing))!=direction) ++facing;
    const int cell=user.cell.y*game.stage.width+user.cell.x;
    game.stage.prop_owners.erase(cell);
    if (has_player(game,user.owner)) game.stage.prop_owners[cell]=user.owner;
    tile->prop={PropKind::TensionSpring,6,static_cast<std::uint8_t>(facing),false,tension_arm_ticks};
    return true;
}
