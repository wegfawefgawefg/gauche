#pragma once
#include "../game.hpp"

enum BreakerPhase { BreakerMarch, BreakerPush, BreakerHammer, BreakerCut, BreakerRecover };
constexpr int breaker_shield_health=90;
void init_strikebreaker(Entity& guard);
void step_strikebreaker(Game& game,int slot);
void interrupt_strikebreaker(Entity& guard);
void alert_strikebreakers(Game& game,int victim,Cell source);
bool breaker_blocks(const Entity& guard,Cell source);
void hit_breaker_shield(Game& game,int slot,int damage,Cell source);
bool valid_strikebreaker(const Entity& guard);
