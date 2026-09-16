#include "forest_spider.hpp"
#include "behavior.hpp"
#include "attacks.hpp"

namespace {
Sprite pose(const Entity& spider,int phase) {
    const Sprite base=spider.counter_b==SpiderYoung ? Sprite::ForestSpiderYoung :
        spider.counter_b==SpiderMother ? Sprite::ForestSpiderMother : Sprite::ForestSpider;
    return static_cast<Sprite>(static_cast<int>(base)+phase);
}
void recover(Entity& spider,int ticks) {
    spider.label_a=ForestSpiderRecover;spider.timer_a=ticks;spider.sprite=pose(spider,0);spider.self_light={};
}
}
void set_forest_spider_role(Entity& spider,ForestSpiderRole role) {
    spider.counter_b=role;
    spider.health=spider.max_health=role==SpiderYoung ? 8 : role==SpiderMother ? 160 : 30;
    spider.move_interval=role==SpiderYoung ? 14 : role==SpiderMother ? 26 : 20;
    spider.sprite=pose(spider,0);spider.counter_a=spider.cell.x;spider.label_b=spider.cell.y;
}
void init_forest_spider(Entity& spider) {
    spider.impassable=true;set_forest_spider_role(spider,SpiderAdult);recover(spider,30);
}
float forest_spider_size(const Entity& spider) {
    return spider.counter_b==SpiderYoung ? .625F : spider.counter_b==SpiderMother ? 1.75F : 1;
}
void interrupt_forest_spider(Entity& spider) {
    if (spider.kind==EntityKind::ForestSpider && spider.health>0) recover(spider,45);
}
void forest_spider_timers(Entity& spider) {
    if (spider.kind!=EntityKind::ForestSpider) return;
    if (spider.label_a==ForestSpiderTell &&
        (spider.stun_ticks || spider.sleep_ticks || spider.vitals.rooted || spider.toss.ticks)) recover(spider,45);
    if (spider.label_a==ForestSpiderRecover && !spider.use_flash) spider.sprite=pose(spider,0);
}
void step_forest_spider(Game& game,int slot) {
    auto& spider=game.entities[static_cast<std::size_t>(slot)];
    if (spider.label_a==ForestSpiderTell) {
        if (spider.cell!=spider.point_a || spider.vitals.rooted || spider.toss.ticks) {recover(spider,45);return;}
        if (spider.timer_a) return;
        resolve_enemy_attack(game,slot,spider.counter_b==SpiderMother ? 18 : spider.counter_b==SpiderYoung ? 3 : 8,SoundId::SpiderBite);
        recover(spider,spider.counter_b==SpiderMother ? 90 : 54);
        spider.sprite=pose(spider,2);return;
    }
    if (spider.label_a==ForestSpiderRecover && spider.timer_a) return;
    spider.label_a=ForestSpiderHunt;
    const auto target=enemy_target(game,spider.cell,spider.counter_b==SpiderYoung ? 6 : 9);
    if (target && clear_attack_sight(game,spider.cell,target->cell)) {
        const int reach=spider.counter_b==SpiderMother ? 2 : 1;
        if (distance(spider.cell,target->cell)<=reach) {
            spider.point_a=spider.cell;spider.point_b=target->cell;spider.facing=cardinal_toward(spider.cell,target->cell,spider.facing);
            spider.label_a=ForestSpiderTell;spider.timer_a=spider.counter_b==SpiderMother ? 60 : 36;
            spider.sprite=pose(spider,1);spider.self_light={24,12,8};emit_sound(game,SoundId::SpiderWarn,spider.cell);
        } else pursue(game,slot,target->cell);
    } else if (distance(spider.cell,{spider.counter_a,spider.label_b})>5) approach(game,slot,{spider.counter_a,spider.label_b});
    else if ((game.tick+spider.generation)%90==0) wander(game,slot);
}
