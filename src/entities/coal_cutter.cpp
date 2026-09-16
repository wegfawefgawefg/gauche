#include "coal_cutter.hpp"
#include "../props/conveyor.hpp"
#include <algorithm>

// SLOTS: facing points at the single adjacent ore face; rear cell is the outlet.
// label_a working/blocked/exhausted; timer_a cutting beat; counter_a rock chipped
// toward the next lump. Coal belongs to the wall, never an infinite machine stock.
namespace {
void state(Game& game,Entity& cutter,CutterState next) {
    if (cutter.label_a!=next && next!=CutterWorking)
        emit_sound(game,next==CutterBlocked ? SoundId::CutterJam : SoundId::CutterEmpty,cutter.cell);
    cutter.label_a=next;
    if (next!=CutterWorking) cutter.timer_a=0;
}
bool outlet_clear(const Game& game,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    if (!tile || !walkable(*tile) || !live_belt(tile->prop) || tile->prop.growth_ticks>0) return false;
    for (const Entity& actor:game.entities)
        if (actor.kind!=EntityKind::None && actor.cell==cell &&
            (actor.impassable || actor.kind==EntityKind::GroundItem)) return false;
    return true;
}
}
void init_coal_cutter(Entity& cutter) {
    cutter.health=cutter.max_health=100;cutter.impassable=cutter.hard_blocker=true;
    cutter.sprite=Sprite::CoalCutter;cutter.facing={0,-1};cutter.timer_a=60;
}
void step_coal_cutter(Game& game,int slot) {
    Entity& cutter=game.entities[static_cast<std::size_t>(slot)];
    if (cutter.health<=0) return;
    const Cell face=cutter.cell+cutter.facing,outlet=cutter.cell-cutter.facing;
    Tile* ore=game.stage.at(face);
    if (!ore || ore->kind!=TileKind::Wall || ore->contents!=ItemKind::CoalLump || ore->content_count==0) {
        cutter.counter_a=0;state(game,cutter,CutterExhausted);return;
    }
    if (ore->hp==0 || ore->break_rule==BreakRule::Unbreakable ||
        (ore->break_rule==BreakRule::DigRequired && ore->required_dig_power>2) || !outlet_clear(game,outlet)) {
        state(game,cutter,CutterBlocked);return;
    }
    // Restart with a full visible cutting stroke, including after clearing a jam.
    if (cutter.label_a!=CutterWorking) {cutter.timer_a=60;cutter.label_a=CutterWorking;}
    if (cutter.timer_a>0) return;
    const int damage=std::min(6,static_cast<int>(ore->hp));
    const bool broken=damage==ore->hp;
    const int count=broken ? ore->content_count : cutter.counter_a+damage>=30 ? 1 : 0;
    if (count>0) {
        // Reserve real output before changing the wall. Pool exhaustion pauses
        // extraction; it cannot delete coal or spill it onto some nearby floor.
        Entity* lump=get_entity(game,spawn_entity(game,EntityKind::GroundItem,outlet));
        if (!lump) {state(game,cutter,CutterBlocked);return;}
        lump->ground_item=make_item(ItemKind::CoalLump,count);lump->sprite=Sprite::CoalLump;
        ore->content_count=static_cast<std::uint8_t>(ore->content_count-count);
        if (ore->content_count==0) ore->contents=ItemKind::None;
    }
    hit_terrain(game,face,cutter.cell,damage,2,TileImpact::Strike,false);
    cutter.counter_a=broken ? 0 : (cutter.counter_a+damage)%30;
    cutter.timer_a=60;emit_sound(game,SoundId::CutterCut,face);
    if (broken || ore->content_count==0) state(game,cutter,CutterExhausted);
}
bool valid_coal_cutter(const Entity& cutter) {
    return cutter.kind!=EntityKind::CoalCutter || (cutter.health>0 && cutter.health<=100 && cutter.max_health==100 &&
        cutter.impassable && cutter.hard_blocker && cutter.move_interval==0 && distance({},cutter.facing)==1 &&
        cutter.label_a>=CutterWorking && cutter.label_a<=CutterExhausted && cutter.timer_a>=0 && cutter.timer_a<=60 &&
        cutter.counter_a>=0 && cutter.counter_a<30 && cutter.ground_item.kind==ItemKind::None);
}
