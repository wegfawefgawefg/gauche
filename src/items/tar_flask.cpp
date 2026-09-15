#include "tar_flask.hpp"
#include "cold_flask.hpp"
#include "../surfaces/interaction.hpp"
#include "../projectiles/projectile.hpp"

namespace {
constexpr RegionalItem flask{"Tar Flask",
    "Throw sticky fuel. Slows walkers for 10s; fire burns it. Cold hardens it into safe crust. Water washes it away.",
    Sprite::TarFlask,{1,5,1,0,45,PatternEffect::Utility},
    ItemAction::Throw,8,3,true,0,0,0,0,0,SoundId::TarThrow};
}
const RegionalItem* tar_flask_item(ItemKind kind) {return kind==ItemKind::TarFlask ? &flask : nullptr;}
void tar_flask_impact(Game& game,const Item& item,Cell center) {
    // Freeze the reachable cross before heat or liquid changes its cover.
    const auto cells=cold_flask_cells(game,item,center);
    emit_sound(game,SoundId::TarSplash,center);
    for (Cell cell:cells) pour_surface(game,cell,LiquidKind::Tar,600);
}
