#pragma once
#include "../game.hpp"
// counter_b: adult/hatchling/mother; label_a: hunt/tell/recover.
// point_a/b lock the bite origin and target; counter_a/label_b keep territory x/y.
enum ForestSpiderRole { SpiderAdult, SpiderYoung, SpiderMother };
enum ForestSpiderPhase { ForestSpiderHunt, ForestSpiderTell, ForestSpiderRecover };
void init_forest_spider(Entity& spider);
void set_forest_spider_role(Entity& spider,ForestSpiderRole role);
void step_forest_spider(Game& game,int slot);
void interrupt_forest_spider(Entity& spider);
void forest_spider_timers(Entity& spider);
float forest_spider_size(const Entity& spider);
