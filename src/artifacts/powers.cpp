#include "powers.hpp"
#include "../items/catalog.hpp"
#include "../item_attribute.hpp"
#include <algorithm>
#include <cstdint>

namespace {
int scaled(int base, std::int64_t numerator, std::int64_t denominator=100) {
    if (base<=0) return 0;
    return static_cast<int>(std::clamp<std::int64_t>((base*numerator+denominator-1)/denominator,1,100000000));
}
bool chance(Game& game, std::uint32_t stacks, std::uint32_t divisor) {
    return stacks>0 && random_u32(game) % (stacks+divisor) < stacks;
}
}

bool artifact_stackable(ArtifactKind kind) {
    switch (kind) {
    case ArtifactKind::None: case ArtifactKind::Count: case ArtifactKind::AllPiercing:
    case ArtifactKind::Reflector: case ArtifactKind::Hearth: case ArtifactKind::Oversized:
    case ArtifactKind::Sweeping: case ArtifactKind::Iron: case ArtifactKind::Chef:
    case ArtifactKind::GodHand: return false;
    default: return true;
    }
}
bool artifact_eligible(const Entity& player, ArtifactKind kind) {
    if (kind==ArtifactKind::None || kind==ArtifactKind::Count) return false;
    if (kind==ArtifactKind::CriticalPower && !has_artifact(player,ArtifactKind::CriticalChance)) return false;
    return artifact_stackable(kind) ? artifact_count(player,kind)<max_power_stacks : !has_artifact(player,kind);
}
void grant_artifact(Entity& player, ArtifactKind kind) {
    if (!artifact_eligible(player,kind)) return;
    player.powers[static_cast<std::size_t>(kind)]=artifact_count(player,kind)+1;
    player.artifacts |= 1U<<static_cast<unsigned>(kind);
    if (kind==ArtifactKind::Vitality) {
        const int gain=std::min(20,100000000-player.max_health);
        player.max_health+=gain; player.health=std::min(player.max_health,player.health+gain);
    }
    if (kind==ArtifactKind::GodHand) {
        for (auto& item:player.inventory.slots)
            if (item.kind==player.basic_action) {item=make_item(ItemKind::GodFist);break;}
        player.basic_action=ItemKind::GodFist;
    }
}
int power_damage(const Entity& player, int base) {
    return scaled(base,100+25LL*artifact_count(player,ArtifactKind::StrongArms),
        100+10LL*artifact_count(player,ArtifactKind::QuickHands));
}
int roll_power_damage(Game& game, const Entity& attacker, int base) {
    int damage=power_damage(attacker,base);
    const auto crit=artifact_count(attacker,ArtifactKind::CriticalChance);
    if (crit && random_u32(game)%100 < std::min(100U,5*crit))
        damage=scaled(damage,200+100LL*artifact_count(attacker,ArtifactKind::CriticalPower));
    return damage;
}
int defend_with_powers(Game& game, Entity& target, int damage, bool direct) {
    if (!direct) return damage;
    if (chance(game,artifact_count(target,ArtifactKind::Dodge),19)) {
        emit_sound(game,SoundId::PowerDodge,target.cell); return 0;
    }
    return scaled(damage,10,10LL+artifact_count(target,ArtifactKind::Armor));
}
int power_healing(const Entity& player, int base) { return scaled(base,100+25LL*artifact_count(player,ArtifactKind::Medic)); }
int power_gold(const Entity& player, int base) { return scaled(base,100+25LL*artifact_count(player,ArtifactKind::Golddigger)); }
int power_move_interval(Entity& player) {
    const auto speed=artifact_count(player,ArtifactKind::FleetFeet);
    if (!speed) return player.move_interval;
    // Carry fractional ticks so the first +5% matters, even with a seven-tick base step.
    const auto duration=std::max<std::int64_t>(1000000,player.move_interval*100000000LL/(100+5LL*speed));
    const auto total=duration+player.move_fraction;
    player.move_fraction=static_cast<int>(total%1000000);
    return static_cast<int>(total/1000000);
}
bool power_attack_item(ItemKind kind) {
    const auto* spec=regional_item(kind);
    return item_is_melee(kind) || item_is_gun(kind) || kind==ItemKind::Bomb ||
        (spec && (spec->action==ItemAction::Throw || spec->pattern.damage>0)) ||
        (kind>=ItemKind::Slap && kind<=ItemKind::GodFist) || kind==ItemKind::CrushShield;
}
void step_player_powers(Game& game, Entity& player) {
    if (player.kind!=EntityKind::Player) return;
    const auto gain=1000000LL*(100+20LL*artifact_count(player,ArtifactKind::QuickHands))/
        (100+15LL*artifact_count(player,ArtifactKind::StrongArms));
    const auto total=player.action_fraction+gain;
    player.action_steps=static_cast<int>(total/1000000);
    player.action_fraction=static_cast<int>(total%1000000);
    if (player.health<=0) return;
    const auto regen=artifact_count(player,ArtifactKind::Regeneration);
    if (player.health<player.max_health && regen) {
        const auto progress=player.regen_progress+static_cast<std::int64_t>(regen);
        player.health=std::min(player.max_health,player.health+static_cast<int>(progress/1200));
        player.regen_progress=static_cast<int>(progress%1200);
    } else player.regen_progress=0;
    (void)game;
}
void improve_pickup(const Entity& player, Item& item) {
    const auto level=artifact_count(player,ArtifactKind::Technical);
    if (level<=item.technical_level) return;
    // Improve from the original catalog cap, once per attained level. Wear is preserved.
    const Item base=make_item(item.kind,1,item.attribute);
    if (base.max_durability>0) {
        const int cap=scaled(base.max_durability,100+50LL*level);
        item.durability=std::max(1,cap-(item.max_durability-item.durability));item.max_durability=cap;
    }
    if (base.max_uses>0) {
        const int cap=scaled(base.max_uses,100+50LL*level);
        item.uses=std::max(1,cap-(item.max_uses-item.uses));item.max_uses=cap;
    }
    if (base.max_durability || base.max_uses) item.technical_level=level;
}
bool preserve_throw(Game& game, const Entity& player, ItemKind kind) {
    const auto* spec=regional_item(kind);
    return (kind==ItemKind::Bomb || (spec && spec->action==ItemAction::Throw)) &&
        chance(game,artifact_count(player,ArtifactKind::Reusable),4);
}
