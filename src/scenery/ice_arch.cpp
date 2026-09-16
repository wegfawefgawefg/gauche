#include "ice_arch.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/temperature.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

void step_ice_arch(Game& game,RoofSpan& roof) {
    const Cell first=roof_cell(roof,0,1),last=roof_cell(roof,roof.length-1,1);
    const auto supported=[&](Cell c) {
        const auto& p=game.stage.at_or_border(c).prop;
        return p.kind==PropKind::IceArchFoot && !p.broken;
    };
    const bool unsupported=!supported(first) || !supported(last);
    bool heated=false;
    for (int a=0;a<roof.length;++a) heated|=hot_cell(game,roof_cell(roof,a,1));
    if (!heated && !unsupported) return;
    const Cell effect=roof_cell(roof,roof.length/2,1);
    if (roof.hp==roof_health(roof.kind)) emit_sound(game,SoundId::PillarCrack,effect);
    roof.hp=static_cast<std::uint8_t>(std::max(0,static_cast<int>(roof.hp)-(unsupported ? 4 : 1)));
    if (heated && game.tick%60==0) {
        const Cell drip=roof_cell(roof,static_cast<int>(game.tick/60%roof.length),1);
        const auto liquid=game.stage.at_or_border(drip).surface.liquid;
        if (liquid==LiquidKind::None || liquid==LiquidKind::Water) pour_surface(game,drip,LiquidKind::Water,180);
        emit_sound(game,SoundId::RoofMelt,drip);
    }
    if (roof.hp) return;
    // Retire only our feet. Occupants, loot, floor materials and replacement
    // props survive collapse; the open span never becomes a blocking wall.
    for (Cell cell:{first,last}) {
        auto& prop=game.stage.at(cell)->prop;
        if (prop.kind!=PropKind::IceArchFoot || prop.broken) continue;
        hit_prop(game,cell,prop.hp,effect);
    }
    emit_sound(game,heated ? SoundId::PillarMelt : SoundId::RoofBreak,effect);
    if (game.impact_count<static_cast<int>(game.impacts.size()))
        game.impacts[static_cast<std::size_t>(game.impact_count++)]={effect,effect,Sprite::IceRubble,0,true,PropKind::IceRubble};
}
