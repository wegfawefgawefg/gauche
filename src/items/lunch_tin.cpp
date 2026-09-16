#include "lunch_tin.hpp"
#include "../artifacts/hearth.hpp"
#include "../entities/hearing.hpp"
#include "../projectiles/recoverable.hpp"

namespace {
constexpr RegionalItem lunch{"Lunch Tin",
    "Two meals: each heals 10 HP over 3s. Damage stops the remaining healing. When empty, throw the tin to five tiles for a loud distraction; pick it up again. Cannot refill.",
    Sprite::LunchTin,{0,0,0,0,180,PatternEffect::Heal,false,0,10},
    ItemAction::Food,9,1,false,0,2,0,0,0,SoundId::LunchEat};
}
const RegionalItem* lunch_tin_item(ItemKind kind) {return kind==ItemKind::LunchTin ? &lunch : nullptr;}

// loaded is actual remaining portions (0..2), not ammo or renewable charges.
// Starting a meal spends one portion; interruption discards only pending healing.
// The empty object changes its use, but keeps its kind and survives its flight.
bool use_lunch_tin(Game& game,int slot,Cell direction) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Item& tin=*user.inventory.held();
    if (tin.kind!=ItemKind::LunchTin || tin.count!=1 || tin.cooldown>0 || user.health<=0) return false;
    if (tin.loaded==0) {
        if (distance({},direction)!=1 || !launch_recoverable(game,slot,tin,direction)) return false;
        tin={};user.use_flash=8;emit_sound(game,SoundId::LunchThrow,user.cell);return true;
    }
    if (tin.loaded<0 || tin.loaded>2 || (user.health>=user.max_health && !hearth_meal_needed(game,user,tin.kind))) return false;
    const ItemPattern pattern=item_pattern(tin);
    if (!begin_recovery(user,RecoveryKind::Meal,pattern.heal)) return false;
    --tin.loaded;tin.cooldown=item_pattern(tin).cooldown;user.use_flash=8;
    share_hearth_meal(game,user,tin.kind);
    emit_sound(game,SoundId::LunchEat,user.cell);return true;
}
void land_lunch_tin(Game& game,Cell cell) {
    make_noise(game,cell,8);
    emit_sound(game,SoundId::LunchClatter,cell);
}
