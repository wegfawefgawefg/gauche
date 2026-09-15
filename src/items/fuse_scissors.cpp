#include "fuse_scissors.hpp"
#include "../projectiles/exposed_fuse.hpp"

namespace {
constexpr RegionalItem scissors{"Fuse Scissors",
    "Snip a landed exposed fuse at your feet or ahead. Recovers the bomb if it fits. Sealed fuses resist.",
    Sprite::FuseScissors,{0,1,0,0,24,PatternEffect::Utility},
    ItemAction::Material,8,1,false,12,0,0,0,0,SoundId::FuseSnip};
}
const RegionalItem* fuse_scissors_item(ItemKind kind) { return kind==ItemKind::FuseScissors ? &scissors : nullptr; }
bool snip_fuse(Game& game,int slot,Cell direction) {
    auto& user=game.entities[static_cast<std::size_t>(slot)];
    int chosen=-1,shortest=10000;
    for (int i=0;i<max_entities;++i) {
        const auto& shot=game.entities[static_cast<std::size_t>(i)];
        if (!exposed_fuse(shot) || shot.counter_a!=0 || shot.timer_a<=0 ||
            (shot.cell!=user.cell && shot.cell!=user.cell+direction)) continue;
        if (shot.timer_a<shortest) { chosen=i; shortest=shot.timer_a; }
    }
    if (chosen<0) return false;
    auto& shot=game.entities[static_cast<std::size_t>(chosen)];
    if (!insert_item(user.inventory,shot.ground_item)) return false;
    remove_entity(game,{chosen,shot.generation});
    return true;
}
