#include "glow_slag.hpp"
#include "heat_siphon.hpp"
#include "../surfaces/interaction.hpp"

namespace {
constexpr RegionalItem slag{"Glow Slag",
    "Recoverable throw to six, 4 damage. Glows and warms nearby ice for 20s. Secondary draws real heat from ahead; lava reheats fully. Water/cold quenches. Moths steal its heat.",
    Sprite::GlowSlag,{1,6,0,4,30,PatternEffect::Damage,true},
    ItemAction::Throw,6,1,true,0,glow_slag_ticks,0,0,0,SoundId::GlowThrow};
}
const RegionalItem* glow_slag_item(ItemKind kind) {return kind==ItemKind::GlowSlag ? &slag : nullptr;}
bool glowing_slag(const Item& item) {return item.kind==ItemKind::GlowSlag && item.count>0 && item.loaded>0;}

// loaded is remaining heat, never ammunition. Every physical copy cools,
// including stowed gear and cargo. No passive recharge: its own heat cannot
// sustain itself or multiply by warming another lump beside it.
void step_glow_slag(Game& game,Item& item,Cell cell,bool wet) {
    if (!glowing_slag(item)) return;
    item.loaded=wet ? 0 : item.loaded-1;
    if (!item.loaded) emit_sound(game,SoundId::GlowCool,cell);
}
bool reheat_glow_slag(Game& game,int slot) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Item& item=*user.inventory.held();
    if (item.kind!=ItemKind::GlowSlag || item.count!=1 || item.cooldown>0 ||
        item.loaded>=glow_slag_ticks || distance({},user.facing)!=1 ||
        surface_wet(game.stage.at_or_border(user.cell))) return false;
    const Cell source=user.cell+user.facing;
    const int missing=glow_slag_ticks-item.loaded;
    const Tile* tile=game.stage.at(source);
    if (!tile) return false;
    const int heat=tile->kind==TileKind::Lava ? missing : extract_heat(game,source,missing);
    if (heat<=0) return false;
    item.loaded+=heat;item.cooldown=30;user.use_flash=8;
    emit_sound(game,SoundId::GlowHeat,user.cell);return true;
}
