#pragma once
#include "../game.hpp"
enum BrawlerPhase { BrawlerHunt, BrawlerWindup, BrawlerRecover };
constexpr bool brawler_kind(EntityKind kind) {
    return kind==EntityKind::FrostGoblin || kind==EntityKind::PipeGuard;
}
void init_brawler(Entity& actor);
void step_brawler(Game& game,int slot);
void interrupt_brawler(Entity& actor);
bool valid_brawler(const Entity& actor);
