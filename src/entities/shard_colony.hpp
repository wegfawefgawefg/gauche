#pragma once
#include "../game.hpp"
#include <array>
#include <vector>

enum ShardPhase { ShardIdle, ShardCharge, ShardPulse, ShardRecover };
struct ShardGroup { std::array<Handle,3> nodes{}; int count = 0; };
struct ShardLink { Cell from{}, to{}; std::vector<Cell> cells; };
void init_shard_node(Game& game, Entity& node);
Handle spawn_shard_colony(Game& game, const std::array<Cell,3>& cells);
ShardGroup shard_group(const Game& game, Handle colony);
std::vector<ShardLink> shard_links(const Game& game, const ShardGroup& group);
void interrupt_shard_node(Entity& node);
void step_shard_state(Entity& node);
void step_shard_colony(Game& game, int slot);
void drop_shard_colony(Game& game, const Entity& node);
