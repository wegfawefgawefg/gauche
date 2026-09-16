#pragma once
#include "../game.hpp"

enum DogPhase { DogHunt, DogWindup, DogRecover };
constexpr int dog_windup_ticks=24, dog_recovery_ticks=30;
void interrupt_dog_bite(Entity& dog);
bool valid_dog_bite(const Entity& dog);
