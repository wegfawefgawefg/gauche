#include "effigy_mask.hpp"
#include "action.hpp"

namespace {
constexpr RegionalItem mask{"Effigy Mask",
    "Hold Use and stand still: the mask watches behind you, holding effigies still. Front gaze remains. 12s total wear.",
    Sprite::EffigyMask,{0,7,0,0,0,PatternEffect::Utility},
    ItemAction::Material,28,1,false,12,0,0,0,0,SoundId::MaskRaise};
}
const RegionalItem* effigy_mask_item(ItemKind kind) {
    return kind==ItemKind::EffigyMask ? &mask : nullptr;
}

// ITEM SLOTS: spare = worn ticks in the current one-second charge (0..59).
// PLAYER SLOTS: label_b = mask_action_label; point_b = stationary stance cell.
// Partial wear survives stowing/trading, so tapping cannot create free observation.
int effigy_mask_ticks(const Item& item) { return item.uses*60-item.spare; }

bool effigy_mask_active(const Entity& user) {
    const Item& item=*user.inventory.held();
    return user.kind==EntityKind::Player && user.health>0 && user.sleep_ticks==0 && user.stun_ticks==0 &&
        item.kind==ItemKind::EffigyMask && item.opened && item.uses>0 &&
        user.label_b==mask_action_label && user.cell==user.point_b;
}

void stow_effigy_mask(Item& item,bool held) {
    if (item.kind==ItemKind::EffigyMask && !held) item.opened=false;
}

bool step_effigy_mask(Game& game,int slot,const Input& input) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Item& item=*user.inventory.held();
    if (item.kind!=ItemKind::EffigyMask) {
        if (user.label_b==mask_action_label) cancel_item_action(user);
        return false;
    }
    const bool displaced=user.label_b==mask_action_label && item.opened && user.point_b!=user.cell;
    if (!input.use || input.move!=Cell{} || input.cancel_use || input.pickup || input.drop || input.interact ||
        user.health<=0 || user.sleep_ticks>0 || user.stun_ticks>0 || item.uses<=0 || displaced) {
        if (item.opened) emit_sound(game,SoundId::MaskLower,user.cell);
        item.opened=false;
        if (user.label_b!=0) cancel_item_action(user);
        return true;
    }
    if (user.label_b!=mask_action_label || !item.opened) {
        cancel_item_action(user);
        user.label_b=mask_action_label; user.point_b=user.cell;
        item.opened=true;
        emit_sound(game,SoundId::MaskRaise,user.cell);
    }
    if (++item.spare>=60) {
        item.spare=0;
        if (--item.uses==0) {
            item={}; cancel_item_action(user);
            emit_sound(game,SoundId::MaskSpent,user.cell);
        } else emit_sound(game,SoundId::MaskWhisper,user.cell);
    }
    return true;
}
