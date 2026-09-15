#include "industrial_melee.hpp"
#include "../entities/boiler_tank.hpp"
#include <algorithm>

namespace {
constexpr RegionalItem hammer{"Press Hammer", "Slow 32-damage shove. Double damage to blocking props; dig power 1. Walls can crush shoved victims.",
    Sprite::PressHammer,{1,1,0,32,48,PatternEffect::Damage},
    ItemAction::Melee,25,1,false,0,0,0,0,1,SoundId::PressHeave,30};
constexpr RegionalItem mallet{"Rubber Mallet", "Quick 3-damage shove; walls can crush. Boiler taps shed 40 pressure; sealed outlets refuse. 70 condition.",
    Sprite::RubberMallet,{1,1,0,3,18,PatternEffect::Damage},
    ItemAction::Melee,9,1,false,0,0,0,0,0,SoundId::MalletSwing,70};
}

const RegionalItem* industrial_melee_item(ItemKind kind) {
    if (kind==ItemKind::PressHammer) return &hammer;
    if (kind==ItemKind::RubberMallet) return &mallet;
    return nullptr;
}

bool tap_boiler(Game& game,int slot) {
    Entity& tank=game.entities[static_cast<std::size_t>(slot)];
    if (tank.kind!=EntityKind::BoilerTank || tank.health<=0) return false;
    if (tank.timer_b>0) return true; // A sealed outlet cannot vent pressure.
    const int previous=tank.counter_a;
    tank.counter_a=std::max(0,tank.counter_a-40);
    if (tank.label_a==BoilerTell && tank.counter_a<25) {
        tank.label_a=BoilerVent; tank.timer_a=30;
        tank.self_light={};
    }
    tank.sprite=tank.timer_b>0 ? Sprite::BoilerPlugged : tank.label_a==BoilerTell ? Sprite::BoilerTell :
        tank.counter_a>=60 ? Sprite::BoilerHot : Sprite::BoilerTank;
    if (previous>0) emit_sound(game,SoundId::MalletVent,tank.cell);
    return true;
}
