#include "icicle_spider.hpp"
#include "behavior.hpp"
#include "attacks.hpp"

namespace {
// SLOTS: label_a phase; counter_a owns a woven span; timer_a work/bite/recovery,
// timer_b hunt timeout. point_a/b wall anchors, reused for a fixed bite only after
// releasing the strand. entity_a is the generation-checked creature caught by it.
void recover(Entity& spider, int ticks) {
    spider.label_a = SpiderRecover; spider.timer_a = ticks;
    spider.sprite = Sprite::SpiderRest; spider.self_light = {};
}

void begin_bite(Game& game, Entity& spider, Cell target) {
    clear_spider_strand(game,spider);
    spider.point_a = spider.cell; spider.point_b = target;
    spider.facing = target-spider.cell;
    spider.label_a = SpiderBite; spider.timer_a = 27;
    spider.sprite = Sprite::SpiderBite; spider.self_light = {20,27,29};
    emit_sound(game,SoundId::SpiderWarn,spider.cell);
}
}

void init_icicle_spider(Entity& spider) {
    spider.health = spider.max_health = 34;
    spider.move_interval = 9; spider.impassable = true;
    spider.sprite = Sprite::IcicleSpider;
    spider.label_a = SpiderRecover; spider.timer_a = 30;
}

void interrupt_icicle_spider(Game& game, Entity& spider) {
    if (spider.kind != EntityKind::IcicleSpider) return;
    if (spider.health <= 0) { clear_spider_strand(game,spider); return; }
    // An existing strand survives a nonfatal hit or sleep; unfinished work does not.
    if (spider.counter_a) { spider.sprite = Sprite::IcicleSpider; spider.label_a = SpiderWait; }
    else recover(spider,60);
}

void step_icicle_spider(Game& game, int slot) {
    Entity& spider = game.entities[static_cast<std::size_t>(slot)];
    if (spider.label_a == SpiderBite) {
        if (spider.cell != spider.point_a || spider.vitals.rooted > 0) { recover(spider,60); return; }
        if (spider.timer_a == 0) {
            resolve_enemy_attack(game,slot,12,SoundId::SpiderBite);
            if (spider.health > 0) recover(spider,54);
        }
        return;
    }
    if (spider.label_a == SpiderRecover && spider.timer_a > 0) return;
    const Entity* prey = spider.label_a == SpiderHunt ? get_entity(game,spider.entity_a) : nullptr;
    const auto nearby = enemy_target(game,spider.cell,1);
    const Cell target = prey && prey->health > 0 ? prey->cell : nearby ? nearby->cell : spider.cell;
    if (distance(spider.cell,target) == 1 && clear_attack_sight(game,spider.cell,target)) {
        begin_bite(game,spider,target); return;
    }
    if (spider.label_a == SpiderRecover) {
        if (!find_spider_site(game,slot)) { recover(spider,90); return; }
        spider.label_a = SpiderSeek; spider.timer_b = 180; spider.sprite = Sprite::IcicleSpider;
    }
    if (spider.label_a == SpiderHunt) {
        if (!prey || prey->health <= 0 || spider.timer_b == 0 ||
            !clear_attack_sight(game,spider.cell,prey->cell)) { recover(spider,90); return; }
        pursue(game,slot,prey->cell);
        return;
    }
    if (spider.label_a == SpiderWait) return;
    const auto cells = spider_strand_cells(spider);
    if (cells.empty()) { recover(spider,90); return; }
    const Cell stand = cells.front();
    if (spider.label_a == SpiderWeave) {
        if (spider.cell != stand || spider.vitals.rooted > 0) { recover(spider,60); return; }
        if (spider.timer_a > 0) return;
        if (!weave_spider_strand(game,spider)) { recover(spider,90); return; }
        spider.label_a = SpiderWait; spider.sprite = Sprite::IcicleSpider;
        return;
    }
    if (spider.cell == stand) {
        spider.label_a = SpiderWeave; spider.timer_a = 60;
        spider.facing = cells[1]-stand; spider.sprite = Sprite::SpiderWeave;
        emit_sound(game,SoundId::SpiderWeave,stand);
    } else if (spider.timer_b == 0) recover(spider,90);
    else if (spider.move_wait == 0) {
        const auto next = next_route_cell(game,slot,stand,192);
        if (!next || !willing_step(game,slot,*next)) recover(spider,90);
    }
}
