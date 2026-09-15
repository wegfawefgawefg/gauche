#include "foreman_whistle.hpp"
#include "../entities/mine_crew.hpp"
#include "../entities/hearing.hpp"
#include <algorithm>

namespace {
constexpr RegionalItem whistle{"Foreman's Whistle",
    "Direct idle miners for 4s. Anger and foreman calls override. Answered calls wake sleepers.",
    Sprite::ForemanWhistleItem,{1,4,0,0,90,PatternEffect::Utility,true},
    ItemAction::Material,18,1,false,6,0,0,0,0,SoundId::WorkCall};
}
const RegionalItem* foreman_whistle_item(ItemKind kind) {
    return kind==ItemKind::ForemanWhistle ? &whistle : nullptr;
}
bool use_foreman_whistle(Game& game,int slot,Cell direction) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    const int reach=item_pattern(*user.inventory.held()).maximum;
    const Cell center=user.cell+Cell{direction.x*reach,direction.y*reach};
    const Cell side{-direction.y,direction.x};
    const auto heard=audible_cells(game,user.cell,8);
    bool changed=false;
    for (Entity& worker:game.entities) {
        if (worker.kind!=EntityKind::Pickhand || worker.health<=0 || worker.attack_wait>0 ||
            worker.sleep_ticks>0 || worker.stun_ticks>0 || worker.vitals.rooted>0 ||
            worker.label_a!=CrewReady || std::find(heard.begin(),heard.end(),worker.cell)==heard.end()) continue;
        const Entity* leader=get_entity(game,worker.entity_a);
        // A real order currently being sounded cannot be drowned out by repeated use.
        if (leader && leader->health>0 && leader->kind==EntityKind::ShiftForeman &&
            leader->label_a==CrewWhistle && leader->sleep_ticks==0 && leader->stun_ticks==0 &&
            distance(leader->cell,worker.cell)<=8) {
            const auto orders=audible_cells(game,leader->cell,8);
            if (std::find(orders.begin(),orders.end(),worker.cell)!=orders.end()) continue;
        }
        const int lane=std::clamp(worker.counter_b,-1,1);
        const Cell target=center+Cell{side.x*lane,side.y*lane};
        const Tile* tile=game.stage.at(target);
        if (!tile || tile->kind==TileKind::Lava || (!walkable(tile->kind) && !crew_diggable(*tile))) continue;
        worker.point_c=target; worker.label_c=PlayerWorkOrder; worker.timer_c=240;
        worker.move_wait=std::min(worker.move_wait,6);
        if (!changed) emit_sound(game,SoundId::WorkAnswer,worker.cell);
        changed=true;
    }
    // Inapplicable calls spend no charge and do not become an unlimited free bell.
    if (changed) make_noise(game,user.cell,8);
    return changed;
}
