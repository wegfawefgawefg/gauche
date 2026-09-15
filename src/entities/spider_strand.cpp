#include "icicle_spider.hpp"
#include "../surfaces/temperature.hpp"
#include "../surfaces/interaction.hpp"

#include <algorithm>

std::vector<Cell> spider_strand_cells(const Entity& spider) {
    std::vector<Cell> cells;
    const Cell delta = spider.point_b-spider.point_a;
    const int length = distance(spider.point_a,spider.point_b);
    // BOUNDS: Endpoints are walls; at most six intervening floor cells belong to a strand.
    if ((delta.x != 0 && delta.y != 0) || length < 3 || length > 7) return cells;
    const Cell step{delta.x/length,delta.y/length};
    for (int i=1;i<length;++i) cells.push_back(spider.point_a+Cell{step.x*i,step.y*i});
    return cells;
}

bool spider_span_clear(const Game& game, const Entity& spider, bool woven) {
    const auto cells = spider_strand_cells(spider);
    if (cells.empty()) return false;
    for (Cell wall : {spider.point_a,spider.point_b}) {
        const Tile* tile = game.stage.at(wall);
        if (!tile || tile->kind != TileKind::Wall) return false;
    }
    for (Cell cell : cells) {
        const Tile* tile = game.stage.at(cell);
        if (!tile || !walkable(tile->kind) || tile->kind == TileKind::Lava) return false;
        if (woven) {
            if (tile->prop.kind != PropKind::SpiderStrand || tile->prop.broken) return false;
        } else {
            if (tile->prop.kind != PropKind::None && !tile->prop.broken) return false;
            if (surface_wet(*tile) || tile->surface.fire_ticks > 0) return false;
            for (const Entity& other : game.entities) {
                if (&other == &spider || other.kind == EntityKind::None) continue;
                if (other.cell == cell) return false;
                // RESERVATION: A damaged span still belongs to its spider until cleanup.
                if (other.kind != EntityKind::IcicleSpider || !other.counter_a) continue;
                const auto occupied = spider_strand_cells(other);
                if (std::find(occupied.begin(),occupied.end(),cell) != occupied.end()) return false;
            }
        }
    }
    return true;
}

bool weave_spider_strand(Game& game, Entity& spider) {
    if (!spider_span_clear(game,spider,false)) return false;
    const std::uint8_t vertical = static_cast<std::uint8_t>(spider.point_a.x == spider.point_b.x);
    for (Cell cell : spider_strand_cells(spider))
        game.stage.at(cell)->prop = {PropKind::SpiderStrand,1,vertical,false};
    spider.counter_a = 1;
    emit_sound(game,SoundId::SpiderSet,spider.cell);
    return true;
}

void clear_spider_strand(Game& game, Entity& spider) {
    if (spider.kind != EntityKind::IcicleSpider || !spider.counter_a) return;
    spider.counter_a = 0;
    for (Cell cell : spider_strand_cells(spider)) {
        Tile* tile = game.stage.at(cell);
        if (tile && tile->prop.kind == PropKind::SpiderStrand) tile->prop = {};
    }
}

bool cut_spider_strand(Game& game, Cell cell, bool burned) {
    Tile* tile = game.stage.at(cell);
    if (!tile || tile->prop.kind != PropKind::SpiderStrand) return false;
    for (Entity& spider : game.entities) {
        if (spider.kind != EntityKind::IcicleSpider || !spider.counter_a) continue;
        const auto cells = spider_strand_cells(spider);
        if (std::find(cells.begin(),cells.end(),cell) == cells.end()) continue;
        clear_spider_strand(game,spider);
        spider.label_a = SpiderRecover; spider.timer_a = 90;
        spider.sprite = Sprite::SpiderRest;
    }
    tile->prop = {};
    emit_sound(game,burned ? SoundId::SpiderBurn : SoundId::SpiderCut,cell);
    return true;
}

void enter_spider_strand(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    if (actor.kind == EntityKind::IcicleSpider) return;
    const Tile* tile = game.stage.at(actor.cell);
    if (!tile || tile->prop.kind != PropKind::SpiderStrand) return;
    // PREY: The first crossing consumes the entire line; standing still cannot retrigger it.
    for (Entity& spider : game.entities) {
        if (spider.kind != EntityKind::IcicleSpider || !spider.counter_a) continue;
        const auto cells = spider_strand_cells(spider);
        if (std::find(cells.begin(),cells.end(),actor.cell) == cells.end()) continue;
        clear_spider_strand(game,spider);
        spider.entity_a = {slot,actor.generation};
        spider.label_a = SpiderHunt; spider.timer_b = 180; spider.move_wait = 0;
        spider.sprite = Sprite::IcicleSpider;
    }
    game.stage.at(actor.cell)->prop = {};
    apply_root(actor,60);
    emit_sound(game,SoundId::SpiderCatch,actor.cell);
}

void step_spider_strand(Game& game, Entity& spider) {
    if (spider.kind != EntityKind::IcicleSpider) return;
    if (spider.health <= 0) { clear_spider_strand(game,spider); return; }
    if ((spider.sleep_ticks > 0 || spider.stun_ticks > 0 || spider.vitals.rooted > 0) &&
        (spider.label_a == SpiderWeave || spider.label_a == SpiderBite))
        interrupt_icicle_spider(game,spider);
    if (!spider.counter_a) return;
    if (!spider_span_clear(game,spider,true) || spider.cell != spider_strand_cells(spider).front()) {
        clear_spider_strand(game,spider);
        spider.label_a = SpiderRecover; spider.timer_a = 90;
        spider.sprite = Sprite::SpiderRest;
        emit_sound(game,SoundId::SpiderCut,spider.cell);
        return;
    }
    for (Cell cell : spider_strand_cells(spider))
        if (hot_cell(game,cell)) { cut_spider_strand(game,cell,true); return; }
}
