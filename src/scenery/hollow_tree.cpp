#include "hollow_tree.hpp"
#include "../world/terrain_material.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

void step_hollow_tree(Game& game,RoofSpan& roof) {
    int shell=0,supports=0;bool burning=false;
    Cell effect=roof.start+Cell{roof.length/2,roof.width/2};
    for (int y=0;y<roof.width;++y) for (int x=0;x<roof.length;++x) {
        const Cell c=roof.start+Cell{x,y};if (!tree_ellipse(roof,c)) continue;
        const auto& tile=game.stage.at_or_border(c);
        if (!tree_ellipse(roof,c,3)) {
            ++shell;
            supports+=tile.kind==TileKind::Wall && wooden_terrain(tile);
        }
        if (tile.surface.fire_ticks) {burning=true;effect=c;}
    }
    const bool unsupported=supports*2<shell;
    if (!burning && !unsupported) return;
    if (roof.hp==roof_health(roof.kind)) emit_sound(game,SoundId::TreeCreak,effect);
    roof.hp=static_cast<std::uint8_t>(std::max(0,static_cast<int>(roof.hp)-(unsupported ? 8 : 2)));
    if (!roof.hp) {
        // The overhead crown retires; its ordinary floor, bodies and loot survive.
        emit_sound(game,SoundId::RoofBreak,effect);
        if (game.impact_count<static_cast<int>(game.impacts.size()))
            game.impacts[static_cast<std::size_t>(game.impact_count++)]={effect,effect,Sprite::ForestTree,0,true};
    }
}
