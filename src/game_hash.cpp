#include "game.hpp"

namespace {

void mix(std::uint64_t& hash, std::uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
}

} // namespace

std::uint64_t game_hash(const Game& game) {
    std::uint64_t hash = 1469598103934665603ULL;
    mix(hash, game.rng);
    mix(hash, game.tick);
    mix(hash, static_cast<std::uint64_t>(game.started));
    mix(hash, static_cast<std::uint64_t>(game.game_over));
    mix(hash, static_cast<std::uint64_t>(game.run.phase));
    mix(hash, game.run.seed);
    mix(hash, static_cast<std::uint64_t>(game.run.floor));
    mix(hash, static_cast<std::uint64_t>(game.run.has_key));
    mix(hash, static_cast<std::uint64_t>(game.run.objective));
    mix(hash, static_cast<std::uint64_t>(game.run.death_policy));
    mix(hash, static_cast<std::uint64_t>(game.run.spawn.x));
    mix(hash, static_cast<std::uint64_t>(game.run.spawn.y));
    mix(hash, static_cast<std::uint64_t>(game.run.exit.x));
    mix(hash, static_cast<std::uint64_t>(game.run.exit.y));
    for (std::size_t owner = 0; owner < game.players.size(); ++owner) {
        mix(hash, static_cast<std::uint64_t>(game.players[owner].slot));
        mix(hash, game.players[owner].generation);
        mix(hash, static_cast<std::uint64_t>(game.run.coins[owner]));
        mix(hash, static_cast<std::uint64_t>(game.run.chosen[owner]));
        mix(hash, static_cast<std::uint64_t>(game.run.shop_ready[owner]));
        mix(hash, static_cast<std::uint64_t>(game.run.online[owner]));
        mix(hash, static_cast<std::uint64_t>(game.run.pending_count[owner]));
        for (const Reward& reward : game.run.offers[owner]) {
            mix(hash, static_cast<std::uint64_t>(reward.kind));
            mix(hash, static_cast<std::uint64_t>(reward.item));
            mix(hash, static_cast<std::uint64_t>(reward.artifact));
            mix(hash, static_cast<std::uint64_t>(reward.amount));
            mix(hash, static_cast<std::uint64_t>(reward.attribute));
        }
        for (const auto& offer : game.run.pending_offers[owner]) {
            for (const Reward& reward : offer) {
                mix(hash, static_cast<std::uint64_t>(reward.kind));
                mix(hash, static_cast<std::uint64_t>(reward.item));
                mix(hash, static_cast<std::uint64_t>(reward.artifact));
                mix(hash, static_cast<std::uint64_t>(reward.amount));
                mix(hash, static_cast<std::uint64_t>(reward.attribute));
            }
        }
    }
    for (ItemKind item : game.run.shop_stock) mix(hash, static_cast<std::uint64_t>(item));
    mix(hash, static_cast<std::uint64_t>(game.stage.width));
    mix(hash, static_cast<std::uint64_t>(game.stage.height));
    for (const Tile& tile : game.stage.tiles) {
        mix(hash, static_cast<std::uint64_t>(tile.kind));
        mix(hash, tile.hp);
    }
    for (const Entity& entity : game.entities) {
        mix(hash, static_cast<std::uint64_t>(entity.kind));
        mix(hash, entity.generation);
        if (entity.kind == EntityKind::None) continue;
        mix(hash, static_cast<std::uint64_t>(entity.cell.x));
        mix(hash, static_cast<std::uint64_t>(entity.cell.y));
        mix(hash, static_cast<std::uint64_t>(entity.facing.x));
        mix(hash, static_cast<std::uint64_t>(entity.facing.y));
        mix(hash, static_cast<std::uint64_t>(entity.sprite));
        mix(hash, static_cast<std::uint64_t>(entity.owner));
        mix(hash, static_cast<std::uint64_t>(entity.health));
        mix(hash, static_cast<std::uint64_t>(entity.max_health));
        mix(hash, static_cast<std::uint64_t>(entity.move_wait));
        mix(hash, static_cast<std::uint64_t>(entity.move_interval));
        mix(hash, static_cast<std::uint64_t>(entity.attack_wait));
        mix(hash, static_cast<std::uint64_t>(entity.attack_interval));
        mix(hash, static_cast<std::uint64_t>(entity.block_ticks));
        mix(hash, static_cast<std::uint64_t>(entity.burn_ticks));
        mix(hash, static_cast<std::uint64_t>(entity.freeze_ticks));
        mix(hash, static_cast<std::uint64_t>(entity.sleep_ticks));
        mix(hash, static_cast<std::uint64_t>(entity.stun_ticks));
        mix(hash, static_cast<std::uint64_t>(entity.train_cars_left));
        mix(hash, static_cast<std::uint64_t>(entity.train_origin.x));
        mix(hash, static_cast<std::uint64_t>(entity.train_origin.y));
        mix(hash, static_cast<std::uint64_t>(entity.spawn_wait));
        mix(hash, static_cast<std::uint64_t>(entity.birth_tick));
        mix(hash, static_cast<std::uint64_t>(entity.impassable));
        mix(hash, static_cast<std::uint64_t>(entity.hard_blocker));
        mix(hash, static_cast<std::uint64_t>(entity.fixture_open));
        mix(hash, entity.artifacts);
        mix(hash, static_cast<std::uint64_t>(entity.inventory.selected));
        for (const Item& item : entity.inventory.slots) {
            mix(hash, static_cast<std::uint64_t>(item.kind));
            mix(hash, static_cast<std::uint64_t>(item.attribute));
            mix(hash, static_cast<std::uint64_t>(item.count));
            mix(hash, static_cast<std::uint64_t>(item.cooldown));
            mix(hash, static_cast<std::uint64_t>(item.loaded));
            mix(hash, static_cast<std::uint64_t>(item.spare));
            mix(hash, static_cast<std::uint64_t>(item.durability));
            mix(hash, static_cast<std::uint64_t>(item.max_durability));
            mix(hash, static_cast<std::uint64_t>(item.uses));
            mix(hash, static_cast<std::uint64_t>(item.max_uses));
            mix(hash, static_cast<std::uint64_t>(item.opened));
        }
        const Item& ground = entity.ground_item;
        mix(hash, static_cast<std::uint64_t>(ground.kind));
        mix(hash, static_cast<std::uint64_t>(ground.attribute));
        mix(hash, static_cast<std::uint64_t>(ground.count));
        mix(hash, static_cast<std::uint64_t>(ground.cooldown));
        mix(hash, static_cast<std::uint64_t>(ground.loaded));
        mix(hash, static_cast<std::uint64_t>(ground.spare));
        mix(hash, static_cast<std::uint64_t>(ground.durability));
        mix(hash, static_cast<std::uint64_t>(ground.max_durability));
        mix(hash, static_cast<std::uint64_t>(ground.uses));
        mix(hash, static_cast<std::uint64_t>(ground.max_uses));
        mix(hash, static_cast<std::uint64_t>(ground.opened));
    }
    return hash;
}
