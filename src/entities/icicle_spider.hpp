#pragma once
#include "../game.hpp"

enum SpiderPhase { SpiderSeek, SpiderWeave, SpiderWait, SpiderHunt, SpiderBite, SpiderRecover };
void init_icicle_spider(Entity& spider);
void step_icicle_spider(Game& game, int slot);
void interrupt_icicle_spider(Game& game, Entity& spider);
bool find_spider_site(Game& game, int slot);

std::vector<Cell> spider_strand_cells(const Entity& spider);
bool spider_span_clear(const Game& game, const Entity& spider, bool woven);
bool weave_spider_strand(Game& game, Entity& spider);
void clear_spider_strand(Game& game, Entity& spider);
bool cut_spider_strand(Game& game, Cell cell, bool burned = false);
void enter_spider_strand(Game& game, int slot);
void step_spider_strand(Game& game, Entity& spider);
