#include "mold_key.hpp"

namespace {
constexpr RegionalItem key{"Mold Key",
    "Open a casting mold; pick up its cargo. Eight turns. Reusable mold. Smashing it also frees the cargo.",
    Sprite::MoldKey,{1,1,0,0,30,PatternEffect::Utility},ItemAction::Material,8,1,false,8,0,0,0,0,SoundId::MoldUnlock};
}
const RegionalItem* mold_key_item(ItemKind kind) {return kind==ItemKind::MoldKey ? &key : nullptr;}
bool unlock_casting_mold(Game& game,int slot,Cell direction) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    if (distance({},direction)!=1 || user.inventory.held()->kind!=ItemKind::MoldKey) return false;
    for (Entity& mold:game.entities) {
        if (mold.kind!=EntityKind::CastingMold || mold.health<=0 || mold.fixture_open || mold.cell!=user.cell+direction) continue;
        mold.fixture_open=true;mold.sprite=Sprite::MoldOpen;return true;
    }
    return false;
}
