#include "steam_lance.hpp"
#include "heated_water.hpp"
#include "kettle.hpp"
#include "../surfaces/interaction.hpp"
#include "../surfaces/temperature.hpp"

namespace {
constexpr RegionalItem lance{"Steam Lance",
    "Fill at water; heat nearby for 1.5s. Wind up a two-cell scald. Cold water douses. Reusable. Hits interrupt.",
    Sprite::SteamLance,{1,2,0,18,45,PatternEffect::Damage},
    ItemAction::Material,24,1,false,0,0,0,0,0,SoundId::LanceWindup};
}
const RegionalItem* steam_lance_item(ItemKind kind) {return kind==ItemKind::SteamLance ? &lance : nullptr;}
bool use_steam_lance(Game& game,int slot,Cell direction) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];Item& held=*user.inventory.held();
    if (held.kind!=ItemKind::SteamLance || distance({},direction)!=1) return false;
    const Cell source=user.cell;
    if (held.loaded==0) {
        if (!fresh_water(game,source+direction) && !fresh_water(game,source)) return false;
        held.loaded=1;held.spare=0;emit_sound(game,SoundId::LanceFill,source);return true;
    }
    // Snapshot cover before water changes it. Body targets do not stop the thrust.
    const Item item=held;const auto cells=kettle_cells(game,item,source,direction);
    if (cells.empty()) return false;
    held.loaded=held.spare=0;
    const int damage=item_pattern(item).damage;
    emit_sound(game,item.loaded==2 ? SoundId::LanceJet : SoundId::LancePour,source);
    for (Cell cell:cells) {
        if (damage>0) for (int victim=0;victim<max_entities;++victim) {
            const Entity& target=game.entities[static_cast<std::size_t>(victim)];
            if (victim!=slot && target.kind!=EntityKind::None && target.health>0 && target.cell==cell)
                damage_entity(game,victim,damage,source,true,{slot,user.generation});
        }
        pour_surface(game,cell,LiquidKind::Water,300);quench_cell(game,cell);
        emit_sound(game,item.loaded==2 ? SoundId::LanceScald : SoundId::LanceSplash,cell);
    }
    return true;
}
