#include "roof.hpp"
#include "hollow_tree.hpp"
#include "ice_arch.hpp"
#include "../surfaces/temperature.hpp"
#include "../surfaces/interaction.hpp"
#include "../world/terrain_material.hpp"
#include <algorithm>

// Quarter-second material work, independent of render rate and camera reveal.
// Heat consumes one roof once per update, regardless of overlapping heat sources.
void step_roofs(Game& game) {
    if (game.tick%15!=0) return;
    for (RoofSpan& roof:game.stage.roofs) {
        if (!roof.hp) continue;
        if (roof.kind==RoofKind::HollowTree) {step_hollow_tree(game,roof);continue;}
        if (roof.kind==RoofKind::IceArch) {step_ice_arch(game,roof);continue;}
        bool heated=false,burning=false;
        int supports=0;
        Cell effect=roof_cell(roof,roof.length/2,1);
        for (int along=0;along<roof.length;++along) for (int across=0;across<3;++across) {
            const Cell cell=roof_cell(roof,along,across);
            const Tile& tile=game.stage.at_or_border(cell);
            const bool prop_support=((roof.kind==RoofKind::Gantry && tile.prop.kind==PropKind::Grate) ||
                (roof.kind==RoofKind::Container && tile.prop.kind==PropKind::ContainerSide)) && !tile.prop.broken;
            if (across!=1 && ((tile.kind==TileKind::Wall &&
                (roof.kind!=RoofKind::Log || wooden_terrain(tile))) || prop_support)) ++supports;
            if (tile.surface.fire_ticks>0) {burning=true;effect=cell;}
            if (hot_cell(game,cell)) {
                heated=true;
                if (roof.kind==RoofKind::Log) ignite_surface(game,cell);
            }
        }
        const bool unsupported=supports<roof.length;
        int damage=unsupported ? 4 : 0;
        if (roof.kind==RoofKind::Log && burning) damage=std::max(damage,2);
        if (roof.kind==RoofKind::FrozenLog && heated) damage=std::max(damage,1);
        if (!damage) continue;
        const bool first=roof.hp==roof_health(roof.kind);
        roof.hp=static_cast<std::uint8_t>(std::max(0,static_cast<int>(roof.hp)-damage));
        if (first) emit_sound(game,SoundId::RoofCreak,effect);
        if (roof.kind==RoofKind::FrozenLog && game.tick%60==0) {
            // Falling meltwater leaves the floor, existing contents and other liquids intact.
            const Cell cell=roof_cell(roof,static_cast<int>((game.tick/60)%roof.length),1);
            const Tile& tile=game.stage.at_or_border(cell);
            if (tile.surface.liquid==LiquidKind::None || tile.surface.liquid==LiquidKind::Water)
                pour_surface(game,cell,LiquidKind::Water,180);
        }
        if (!roof.hp) {
            if (roof.kind==RoofKind::FrozenLog && heated)
                for (int along=0;along<roof.length;++along) for (int across:{0,2}) {
                    Tile& tile=*game.stage.at(roof_cell(roof,along,across));
                    if (tile.kind==TileKind::Wall && tile.material==TileMaterial::Ice) {
                        tile.kind=TileKind::ShallowWater;tile.hp=0;
                    }
                }
            emit_sound(game,roof.kind==RoofKind::FrozenLog ? SoundId::RoofMelt : SoundId::RoofBreak,effect);
            // Roof fragments are cosmetic. Never replace the occupied passage
            // with blocking rubble or erase its pickups, liquids or ground props.
            if (game.impact_count<static_cast<int>(game.impacts.size()))
                game.impacts[static_cast<std::size_t>(game.impact_count++)]={effect,effect,
                    roof.kind==RoofKind::FrozenLog ? Sprite::IceBlock : roof.kind==RoofKind::Log ? Sprite::RottenLog : roof.kind==RoofKind::Container ? Sprite::ContainerSide : Sprite::GrateH,
                    0,true,roof.kind==RoofKind::FrozenLog ? PropKind::IceBlock : roof.kind==RoofKind::Log ? PropKind::RottenLog : roof.kind==RoofKind::Container ? PropKind::ContainerSide : PropKind::Grate};
        }
    }
}
