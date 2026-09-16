#include "tall_tree.hpp"
#include "interaction.hpp"
#include "../items/folded_bridge.hpp"
#include "../world/water.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>
#include <vector>

namespace {
constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
Cell offset(Cell root,Cell dir,int n) {return root+Cell{dir.x*n,dir.y*n};}
bool gap(TileKind kind) {return kind==TileKind::Chasm || kind==TileKind::Water || shallow_water(kind);}
bool fixture(const Game& game,Cell cell) {
    for (const Entity& e:game.entities) if (e.cell==cell &&
        (e.kind==EntityKind::Key || e.kind==EntityKind::Switch || e.kind==EntityKind::Door ||
         e.kind==EntityKind::Exit || e.kind==EntityKind::EncounterGate || e.hard_blocker)) return true;
    return false;
}
void chips(Game& game,Cell cell,Cell root) {
    if (game.impact_count<static_cast<int>(game.impacts.size()))
        game.impacts[static_cast<std::size_t>(game.impact_count++)]={cell,root,Sprite::FallenLog,24,true,PropKind::FallenLog};
}
void collapse(Game& game,Cell root,Prop& tree) {
    const Cell dir=tree_direction(tree);
    const bool burning=game.stage.at(root)->surface.fire_ticks>0;
    const bool crossing=tree_bridge_length(game,root,dir)>0;
    tree.hp=0;tree.broken=true;
    emit_sound(game,SoundId::TreeFall,root);chips(game,root,root-dir);
    for (int n=1;n<=tree_reach;++n) {
        const Cell cell=offset(root,dir,n);Tile* tile=game.stage.at(cell);
        if (!tile || (!walkable(tile->kind) && !gap(tile->kind)) || fixture(game,cell)) break;
        std::vector<Handle> victims;
        for (int slot=0;slot<max_entities;++slot) {
            const auto& actor=game.entities[static_cast<std::size_t>(slot)];
            if (actor.cell==cell && actor.kind!=EntityKind::None && actor.impassable && actor.health>0)
                victims.push_back({slot,actor.generation});
        }
        hit_prop(game,cell,48,root);
        for (Handle h:victims) if (const auto* actor=get_entity(game,h);actor && actor->cell==cell)
            damage_entity(game,h.slot,48,root,false);
        chips(game,cell,root);
        if (prop_blocks(tile->prop)) break;
        if (gap(tile->kind)) continue; // Unsupported timber sinks; no dry-ground relocation.
        // Never put a blocking section beneath survivors, drops or live props.
        if (entity_at(game,cell,false)>=0 || tile->kind==TileKind::Lava ||
            (tile->prop.kind!=PropKind::None && !tile->prop.broken)) continue;
        tile->prop={};place_prop(game.stage,cell,PropKind::FallenLog,
            static_cast<std::uint8_t>((dir.y!=0 ? 1 : 0)|(crossing ? 4 : 0)));
        if (burning) ignite_surface(game,cell);
    }
    if (crossing && lay_tree_bridge(game,root,dir) && burning)
        for (int n=1;n<=tree_reach;++n) ignite_surface(game,offset(root,dir,n));
}
}
Cell tree_direction(const Prop& prop) {return directions[prop.variant&3U];}
int tree_burn_height(const Prop& prop) {return prop.variant>>2;}
int tree_bridge_length(const Game& game,Cell root,Cell dir) {
    if (distance({},dir)!=1 || !bridge_bank(game.stage.at_or_border(root))) return 0;
    for (int n=1;n<=tree_reach+1;++n) {
        const Cell cell=offset(root,dir,n);const Tile* tile=game.stage.at(cell);
        if (!tile || fixture(game,cell)) return 0;
        if (bridge_bank(*tile)) return n>1 && !prop_blocks(tile->prop) ? n-1 : 0;
        if (n>tree_reach || !gap(tile->kind) || (tile->prop.kind!=PropKind::None && !tile->prop.broken)) return 0;
    }
    return 0;
}
bool lay_tree_bridge(Game& game,Cell root,Cell dir) {
    const int length=tree_bridge_length(game,root,dir);if (!length) return false;
    for (int n=1;n<=length;++n) {
        Tile& tile=*game.stage.at(offset(root,dir,n));
        const auto variant=static_cast<std::uint8_t>((dir.y!=0 ? 1U : 0U)|(static_cast<unsigned int>(tile.kind)<<1));
        tile.kind=TileKind::Bridge;tile.surface={};
        tile.prop={PropKind::LogBridge,24,variant,false};
    }
    return true;
}
bool hit_tall_tree(Game& game,Cell cell,int damage,Cell source) {
    Prop& tree=game.stage.at(cell)->prop;
    if (tree.growth_ticks>0) return true;
    const bool fire=source==cell && game.stage.at(cell)->surface.fire_ticks>0;
    if (fire && tree_burn_height(tree)<12) tree.variant=static_cast<std::uint8_t>(tree.variant+4);
    if (damage<tree.hp) {
        tree.hp=static_cast<std::uint8_t>(tree.hp-damage);
        if (!fire) emit_sound(game,SoundId::TreeChop,cell);
        else if (tree_burn_height(tree)%4==0) emit_sound(game,SoundId::WoodCrack,cell);
        return true;
    }
    // Fire follows the visible planted lean. A cutting blow commits away from
    // its source. Further impacts cannot reroll or skip the warning.
    const Cell dir=source==cell ? tree_direction(tree) : cardinal_toward(source,cell,tree_direction(tree));
    for (int i=0;i<4;++i) if (directions[i]==dir)
        tree.variant=static_cast<std::uint8_t>((tree.variant&~3U)|static_cast<unsigned int>(i));
    tree.hp=1;tree.growth_ticks=tree_warn_ticks+tree_fall_ticks;
    emit_sound(game,SoundId::TreeCreak,cell);return true;
}
void step_tall_tree(Game& game,Cell cell) {
    Prop& tree=game.stage.at(cell)->prop;
    if (!tree.broken && tree.growth_ticks>0 && --tree.growth_ticks==0) collapse(game,cell,tree);
}
bool valid_tree_prop(const Prop& prop) {
    if (prop.kind==PropKind::FallenLog)
        return (prop.variant&~5U)==0 && prop.growth_ticks==0 && (prop.broken || prop.hp>0);
    if (prop.kind!=PropKind::TallTree) return true;
    return prop.variant<52 && (prop.broken || prop.hp>0) && prop.growth_ticks<=tree_warn_ticks+tree_fall_ticks &&
        (prop.growth_ticks==0 || (!prop.broken && prop.hp==1));
}
