#pragma once
#include "../game.hpp"
enum CrawlerPhase { CrawlerRoam, CrawlerCharge, CrawlerRecover };
void init_cable_crawler(Entity& actor);
void step_cable_crawler(Game& game,int slot);
void interrupt_cable_crawler(Entity& actor);
bool valid_cable_crawler(const Entity& actor);
