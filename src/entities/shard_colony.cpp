#include "shard_colony.hpp"
#include "behavior.hpp"
#include "attacks.hpp"

#include <algorithm>

namespace {

// SLOTS: entity_a immutable colony identity (may be stale); point_a committed cell.
// label_a shared phase, timer_a phase clock, timer_b interrupted endpoint cooldown.
// Nodes never locomote; ordinary push/pull can displace them. No c attention slots used.
void phase(Game& game, const ShardGroup& group, int label, int ticks) {
    for (int i=0;i<group.count;++i)
        if (Entity* node = get_entity(game,group.nodes[static_cast<std::size_t>(i)]); node && node->health > 0) {
            node->label_a = label; node->timer_a = ticks;
            if (label == ShardCharge) node->point_a = node->cell;
            step_shard_state(*node);
        }
}

bool threatened(const Game& game, const ShardGroup& group) {
    for (int i=0;i<group.count;++i) {
        const Entity& node = *get_entity(game,group.nodes[static_cast<std::size_t>(i)]);
        const auto target = enemy_target(game,node.cell,7);
        if (target && clear_attack_sight(game,node.cell,target->cell)) return true;
    }
    return false;
}

void pulse(Game& game, const ShardGroup& group) {
    const auto links = shard_links(game,group);
    std::array<bool,max_entities> hit{};
    Cell sounded{-1,-1};
    // SNAPSHOT: Commit all valid connections together; each victim takes one pulse.
    for (const ShardLink& link : links) {
        if (link.from != sounded) emit_sound(game,SoundId::ShardPulse,link.from);
        sounded = link.from;
        for (Cell cell : link.cells)
            for (int slot=0;slot<max_entities;++slot) {
                Entity& actor = game.entities[static_cast<std::size_t>(slot)];
                if (hit[static_cast<std::size_t>(slot)] || actor.health <= 0 || actor.cell != cell) continue;
                if (actor.kind == EntityKind::ShardColony && actor.entity_a ==
                    game.entities[static_cast<std::size_t>(group.nodes[0].slot)].entity_a) continue;
                hit[static_cast<std::size_t>(slot)] = true;
                damage_entity(game,slot,18,link.from);
            }
    }
}

} // namespace

void init_shard_node(Game& game, Entity& node) {
    node.health = node.max_health = 28;
    node.impassable = true;
    node.move_interval = 18; // Allows ordinary displacement/status tools; step has no movement.
    node.entity_a = {static_cast<int>(&node-game.entities.data()),node.generation};
    node.point_a = node.cell; node.sprite = Sprite::ShardNode;
    node.light = {2,300,{126,197,232}};
}

Handle spawn_shard_colony(Game& game, const std::array<Cell,3>& cells) {
    int free = 0;
    for (const Entity& actor : game.entities) free += actor.kind == EntityKind::None;
    if (free < 3) return {};
    for (std::size_t i=0;i<cells.size();++i) {
        const Tile* tile = game.stage.at(cells[i]);
        if (!tile || !walkable(*tile) || entity_at(game,cells[i],false) >= 0) return {};
        for (std::size_t j=0;j<i;++j)
            if (cells[i] == cells[j] || distance(cells[i],cells[j]) > 8 ||
                !clear_attack_sight(game,cells[i],cells[j],false)) return {};
    }
    std::array<Handle,3> nodes{};
    for (std::size_t i=0;i<nodes.size();++i) {
        nodes[i] = spawn_entity(game,EntityKind::ShardColony,cells[i]);
        if (!get_entity(game,nodes[i])) {
            for (std::size_t j=0;j<i;++j) remove_entity(game,nodes[j]);
            return {};
        }
        get_entity(game,nodes[i])->entity_a = nodes[0];
    }
    return nodes[0];
}

void interrupt_shard_node(Entity& node) {
    if (node.kind != EntityKind::ShardColony || node.health <= 0) return;
    node.timer_b = 90; node.sprite = Sprite::ShardDim;
    node.light = {}; node.self_light = {};
}

void step_shard_state(Entity& node) {
    if (node.kind != EntityKind::ShardColony || node.health <= 0) return;
    // CUT: A displaced endpoint never resumes its old connection during this warning.
    if ((node.label_a == ShardCharge || node.label_a == ShardPulse) && node.cell != node.point_a)
        node.timer_b = 90;
    if (node.sleep_ticks > 0 || node.stun_ticks > 0 || node.vitals.rooted > 0) node.timer_b = 90;
    const bool charged = node.timer_b == 0 && (node.label_a == ShardCharge || node.label_a == ShardPulse);
    node.sprite = node.timer_b > 0 || node.label_a == ShardRecover ? Sprite::ShardDim :
        charged ? Sprite::ShardCharged : Sprite::ShardNode;
    node.light = {2,charged ? 700 : node.timer_b > 0 ? 0 : 300,{126,197,232}};
    node.self_light = charged ? LightTint{32,45,48} : LightTint{};
}

void step_shard_colony(Game& game, int slot) {
    Entity& coordinator = game.entities[static_cast<std::size_t>(slot)];
    const ShardGroup group = shard_group(game,coordinator.entity_a);
    // SUCCESSION: A sleeping/dead coordinator cannot freeze the other crystals forever.
    for (int i=0;i<group.count;++i) {
        const Entity& node = *get_entity(game,group.nodes[static_cast<std::size_t>(i)]);
        if (node.sleep_ticks == 0 && node.stun_ticks == 0) {
            if (group.nodes[static_cast<std::size_t>(i)].slot != slot) return;
            break;
        }
    }
    if (coordinator.timer_a > 0) return;
    if (coordinator.label_a == ShardCharge) {
        pulse(game,group);
        phase(game,group,ShardPulse,12);
    } else if (coordinator.label_a == ShardPulse) phase(game,group,ShardRecover,108);
    else if (group.count >= 2 && threatened(game,group)) {
        phase(game,group,ShardCharge,48);
        emit_sound(game,SoundId::ShardCharge,coordinator.cell);
    } else {
        phase(game,group,ShardIdle,30);
    }
}
