#include "heat_siphon.hpp"
#include "action.hpp"
#include "fire.hpp"
#include "kettle.hpp"
#include "../surfaces/temperature.hpp"

#include <algorithm>

namespace {
constexpr RegionalItem siphon{"Heat Siphon",
    "Use: drain fire ahead. Secondary: flame cone. Partial heat weakens shots. Burns friends too.",
    Sprite::HeatSiphon,{1,2,0,8,45,PatternEffect::Damage,false,1,0,false,true},
    ItemAction::Material,34,1,false,0,0,0,0,0,SoundId::SiphonDraw};
}
const RegionalItem* heat_siphon_item(ItemKind kind) {
    return kind==ItemKind::HeatSiphon ? &siphon : nullptr;
}

// STORAGE: loaded is actual stored burn ticks, 0..1800. Partial sources transfer
// exact heat; trading, dropping and stowing preserve it without a separate actor.
bool draw_siphon_heat(Game& game, int slot) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Item& held=*user.inventory.held();
    if (held.kind!=ItemKind::HeatSiphon || held.cooldown>0 || held.loaded>=siphon_capacity) return false;
    const Cell cell=user.cell+user.facing;
    const int amount=extract_heat(game,cell,std::min(siphon_portion,siphon_capacity-held.loaded));
    if (amount==0) return false;
    held.loaded+=amount; held.cooldown=item_pattern(held).cooldown;
    user.use_flash=8;
    emit_sound(game,SoundId::SiphonDraw,cell);
    return true;
}

bool discharge_siphon(Game& game, int slot) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Item& held=*user.inventory.held();
    if (held.kind!=ItemKind::HeatSiphon || held.cooldown>0 || held.loaded<=0) return false;
    // Freeze the same cone geometry used by the kettle before melting cover.
    const auto cells=kettle_cells(game,held,user.cell,user.facing);
    if (cells.empty()) return false;
    const Cell source=user.cell;
    const int heat=std::min(siphon_portion,held.loaded);
    const int damage=item_pattern(held).damage;
    held.loaded-=heat; held.cooldown=item_pattern(held).cooldown;
    user.use_flash=8;
    emit_sound(game,SoundId::SiphonFire,source);
    for (Cell cell : cells) {
        warm_surface(game,cell,std::min(heat,60));
        for (int victim=0; victim<max_entities; ++victim) {
            Entity& actor=game.entities[static_cast<std::size_t>(victim)];
            if (victim==slot || actor.kind==EntityKind::None || actor.health<=0 || actor.cell!=cell) continue;
            damage_entity(game,victim,damage,source);
            const int old_burn=actor.scorch_ticks;
            ignite_struck_actor(game,victim);
            if (actor.scorch_ticks>old_burn) actor.scorch_ticks=std::max(old_burn,heat);
        }
        emit_sound(game,SoundId::SiphonFlame,cell);
    }
    return true;
}

bool step_siphon_action(Game& game, int slot, const Input& input) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    if (user.inventory.held()->kind!=ItemKind::HeatSiphon) return false;
    if (user.label_b!=0) cancel_item_action(user);
    if (input.cancel_use || input.drop || input.interact) return true;
    if (input.reload) discharge_siphon(game,slot);
    else if (input.use) draw_siphon_heat(game,slot);
    return true;
}
