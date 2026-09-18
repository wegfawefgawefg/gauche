#pragma once
#include "renderer/device.hpp"

#include "../game.hpp"

#include <optional>

enum class EncounterPhase { Dormant, Gathering, Countdown, Fighting, Intermission, Released };

void init_encounter(Entity& entity);
void step_encounter(Game& game, int slot);
bool request_encounter(Game& game, Entity& controller);
bool encounter_released(const Game& game, Handle controller);
std::optional<Cell> encounter_join_cell(const Game& game);
void spawn_encounter_wave(Game& game, int slot);
void step_encounter_spawn(Game& game, int slot);
bool make_haunted_floor(Game& game, bool force = false);
void populate_haunted_house(Game& game);
void draw_encounter_status(tr::Renderer* renderer, const Game& game, const Entity& player);
