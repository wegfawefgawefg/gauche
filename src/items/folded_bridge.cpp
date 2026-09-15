#include "folded_bridge.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include "../world/water.hpp"

#include <array>

namespace {
constexpr RegionalItem bridge{"Folded Bridge",
    "Three planks across water; dry banks at both ends. Burnable. Deep falls kill land creatures.",
    Sprite::FoldedBridge,{1,3,0,0,45,PatternEffect::Utility,true},
    ItemAction::Material,24,1,true,0,0,0,0,0,SoundId::BridgeUnfold,30};
bool water(TileKind kind) { return kind==TileKind::Water || shallow_water(kind); }
}

const RegionalItem* folded_bridge_item(ItemKind kind) {
    return kind==ItemKind::FoldedBridge ? &bridge : nullptr;
}

bool bridge_bank(const Tile& tile) {
    return walkable(tile.kind) && tile.kind!=TileKind::Bridge && tile.kind!=TileKind::Lava && !water(tile.kind);
}

// VARIANT: bit 0 vertical, 1..4 original water kind, 5 Durable, 6..7 segment index.
// Indices always increase east/south so either placement direction shares anchors.
TileKind bridge_water(const Prop& plank) { return static_cast<TileKind>((plank.variant>>1)&15U); }

bool valid_bridge_tile(const Tile& tile) {
    if (tile.kind!=TileKind::Bridge && tile.prop.kind!=PropKind::BridgePlank) return true;
    return tile.kind==TileKind::Bridge && tile.prop.kind==PropKind::BridgePlank &&
        !tile.prop.broken && tile.prop.hp>0 && (tile.prop.variant>>6)<3 && water(bridge_water(tile.prop));
}

bool place_folded_bridge(Game& game, int slot) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    const Item& item=*user.inventory.held();
    const Cell direction=user.facing;
    if (item.kind!=ItemKind::FoldedBridge || item.durability<=0 ||
        prop_blocks(game.stage.at_or_border(user.cell).prop) ||
        prop_blocks(game.stage.at_or_border(user.cell+Cell{direction.x*4,direction.y*4}).prop) ||
        distance({},direction)!=1 || !bridge_bank(game.stage.at_or_border(user.cell)) ||
        !bridge_bank(game.stage.at_or_border(user.cell+Cell{direction.x*4,direction.y*4}))) return false;
    std::array<Cell,3> cells{};
    // TRANSACTION: Validate the full span before changing a tile or spending the kit.
    for (int step=1;step<=3;++step) {
        const Cell cell=user.cell+Cell{direction.x*step,direction.y*step};
        const Tile* tile=game.stage.at(cell);
        if (!tile || !water(tile->kind) || (tile->prop.kind!=PropKind::None && !tile->prop.broken) ||
            entity_at(game,cell,false)>=0) return false;
        cells[static_cast<std::size_t>(step-1)]=cell;
    }
    for (int index=0;index<3;++index) {
        Tile& tile=*game.stage.at(cells[static_cast<std::size_t>(index)]);
        const int canonical=direction.x<0 || direction.y<0 ? 2-index : index;
        const auto variant=static_cast<std::uint8_t>((direction.y!=0 ? 1 : 0) |
            (static_cast<unsigned int>(tile.kind)<<1) | (item.attribute==ItemAttribute::Durable ? 32U : 0U) |
            (static_cast<unsigned int>(canonical)<<6));
        tile.kind=TileKind::Bridge; tile.surface={};
        tile.prop={PropKind::BridgePlank,static_cast<std::uint8_t>(item.durability),variant,false};
    }
    return true;
}

void collapse_bridge_plank(Game& game, Cell cell, Cell source) {
    Tile& tile=*game.stage.at(cell);
    tile.kind=bridge_water(tile.prop); tile.prop={}; tile.surface={};
    emit_sound(game,SoundId::BridgeSplash,cell);
    for (int slot=0;slot<max_entities;++slot) {
        const Entity& actor=game.entities[static_cast<std::size_t>(slot)];
        if (actor.kind==EntityKind::None || actor.health<=0 || actor.cell!=cell) continue;
        if (tile.kind!=TileKind::Water) { contact_surface(game,slot); continue; }
        if (actor.kind==EntityKind::GlassEel || actor.kind==EntityKind::BellDiver || actor.kind==EntityKind::SealThief) continue;
        if (wading_actor(actor) || actor.kind==EntityKind::BoilerTank) crush_entity(game,slot,source);
    }
}

void step_bridge_support(Game& game, Cell cell) {
    const Prop& plank=game.stage.at(cell)->prop;
    if (plank.kind!=PropKind::BridgePlank || plank.broken) return;
    const Cell axis=(plank.variant&1U) ? Cell{0,1} : Cell{1,0};
    const int index=plank.variant>>6;
    const Cell start=cell-Cell{axis.x*index,axis.y*index};
    if (!bridge_bank(game.stage.at_or_border(start-axis)) ||
        !bridge_bank(game.stage.at_or_border(start+Cell{axis.x*3,axis.y*3})))
        hit_prop(game,cell,plank.hp,cell);
}
