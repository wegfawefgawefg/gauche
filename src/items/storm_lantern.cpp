#include "storm_lantern.hpp"
#include "action.hpp"

namespace {
constexpr RegionalItem lantern{"Storm Lantern","Aim the beam; hold Use to focus. Secondary shutters it. Stowing saves fuel. Sealed against water.",
    Sprite::StormLantern,{0,0,0,0,12,PatternEffect::Utility},
    ItemAction::Material,20,1,false,0,0,0,0,0,SoundId::LanternOpen,0,
    {8,1600,{255,216,153},LightShape::Cone}};
}

const RegionalItem* storm_lantern_item(ItemKind kind) {
    return kind==ItemKind::StormLantern ? &lantern : nullptr;
}

LightEmitter lantern_light(const Item& item) {
    if (!item.opened || item.loaded<=0) return {};
    LightEmitter light=item.light;
    if (light.shape==LightShape::Beam) { light.radius=12; light.strength=2200; }
    return light;
}

Sprite lantern_sprite(const Item& item) {
    if (!item.opened || item.loaded<=0) return Sprite::StormLantern;
    return item.light.shape==LightShape::Beam ? Sprite::LanternFocused : Sprite::LanternOpen;
}

// ITEM SLOTS: loaded is fuel in ticks; opened is shutter; spare latches Use/Secondary.
// Holding Secondary cannot chatter the shutter. Focus follows held Use, without consuming charges.
bool step_lantern_action(Game& game,int slot,const Input& input) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Item& item=*user.inventory.held();
    if (item.kind!=ItemKind::StormLantern) return false;
    if (user.label_b!=0) cancel_item_action(user);
    const int buttons=input.cancel_use ? 0 : (input.use ? 1 : 0) | (input.reload ? 2 : 0);
    const int pressed=buttons & ~item.spare;
    item.spare=buttons;
    if ((pressed & 2)!=0 && item.cooldown==0) {
        if (item.loaded<=0) emit_sound(game,SoundId::LanternEmpty,user.cell);
        else {
            item.opened=!item.opened; item.cooldown=12;
            emit_sound(game,item.opened ? SoundId::LanternOpen : SoundId::LanternClose,user.cell);
        }
    }
    const LightShape shape=(buttons & 1)!=0 ? LightShape::Beam : LightShape::Cone;
    if (shape!=item.light.shape && item.opened && item.loaded>0)
        emit_sound(game,shape==LightShape::Beam ? SoundId::LanternFocus : SoundId::LanternWide,user.cell);
    item.light.shape=shape;
    if ((pressed & 1)!=0) interact_with_fixture(game,user.owner,user.cell+user.facing);
    return true;
}

void step_lantern_fuel(Game& game,Item& item,Cell cell,bool exposed,bool held) {
    if (item.kind!=ItemKind::StormLantern) return;
    if (!exposed || !held) item.spare=0;
    // STOWED: Its cover closes over the glass. The chosen shutter setting survives re-equipping.
    if (!exposed || !item.opened || item.loaded<=0) return;
    if (--item.loaded==0) {
        item.opened=false;
        emit_sound(game,SoundId::LanternEmpty,cell);
    }
}
