#include "game.hpp"

#include <algorithm>
#include <array>
#include <deque>
#include <utility>
#include <vector>

namespace {

Reward random_reward(Game& game, int category) {
    if (category == 0) {
        constexpr std::array<ItemKind, 8> items{
            ItemKind::Pistol, ItemKind::Bow, ItemKind::Musket, ItemKind::Buckler,
            ItemKind::Bomb, ItemKind::Medkit, ItemKind::RocketLauncher,
            ItemKind::SleepMeds};
        const ItemKind kind = items[random_u32(game) % items.size()];
        return {RewardKind::Item, kind, ArtifactKind::None,
                kind == ItemKind::Bomb ? 3 :
                (kind == ItemKind::Medkit || kind == ItemKind::SleepMeds ? 2 : 1)};
    }
    if (category == 1) {
        constexpr std::array<ArtifactKind, 4> artifacts{
            ArtifactKind::AllPiercing, ArtifactKind::Reflector,
            ArtifactKind::Hearth, ArtifactKind::FleetFeet};
        return {RewardKind::Artifact, ItemKind::None,
                artifacts[random_u32(game) % artifacts.size()], 1};
    }
    return random_u32(game) % 2 == 0 ?
        Reward{RewardKind::Health, ItemKind::None, ArtifactKind::None, 20} :
        Reward{RewardKind::Speed, ItemKind::None, ArtifactKind::None, 1};
}

int price(ItemKind kind) {
    switch (kind) {
    case ItemKind::Bandage: return 12;
    case ItemKind::Buckler: return 35;
    case ItemKind::Pistol: return 45;
    case ItemKind::Musket: return 50;
    case ItemKind::RocketLauncher: return 90;
    default: return 25;
    }
}

void ready_next_floor(Game& game) {
    ++game.run.floor;
    if (game.run.floor > 12) {
        game.run.phase = RunPhase::Won;
        return;
    }
    generate_world_floor(game);
}

} // namespace

void start_run(Game& game, std::uint64_t seed) {
    game = {};
    game.rng = seed == 0 ? 1 : seed;
    game.run.seed = game.rng;
    game.run.floor = 1;
    game.run.phase = RunPhase::Playing;
    generate_world_floor(game);
}

bool interact_with_fixture(Game& game, int owner, Cell target) {
    if (owner < 0 || owner >= 4) return false;
    Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
    if (player == nullptr || player->health <= 0 || distance(player->cell, target) > 1)
        return false;
    for (Entity& fixture : game.entities) {
        if (fixture.cell != target) continue;
        if (fixture.kind == EntityKind::Key && player->cell == target) {
            game.run.has_key = true;
            emit_sound(game, SoundId::Confirm, fixture.cell, false);
            remove_entity(game, {static_cast<int>(&fixture - game.entities.data()),
                                 fixture.generation});
            return true;
        }
        if (fixture.kind == EntityKind::Door && game.run.has_key) {
            fixture.fixture_open = true;
            fixture.impassable = false;
            fixture.hard_blocker = false;
            emit_sound(game, SoundId::BlockLand, fixture.cell);
            return true;
        }
        if (fixture.kind == EntityKind::Exit && game.run.phase == RunPhase::Playing) {
            for (Handle handle : game.players) {
                const Entity* member = get_entity(game, handle);
                if (member != nullptr && member->health > 0 &&
                    distance(member->cell, fixture.cell) > 1) return false;
            }
            finish_floor(game);
            return true;
        }
    }
    return false;
}

void finish_floor(Game& game) {
    game.run.phase = RunPhase::Reward;
    emit_sound(game, SoundId::LevelWin, game.run.exit, false);
    game.run.chosen.fill(false);
    for (std::size_t owner = 0; owner < 4; ++owner) {
        const Entity* player = get_entity(game, game.players[owner]);
        if (player == nullptr || player->health <= 0) {
            game.run.chosen[owner] = true;
            continue;
        }
        auto& offers = game.run.offers[owner];
        for (int index = 0; index < 3; ++index) offers[static_cast<std::size_t>(index)] =
            random_reward(game, index);
        for (int index = 2; index > 0; --index)
            std::swap(offers[static_cast<std::size_t>(index)],
                      offers[random_u32(game) % static_cast<std::uint32_t>(index + 1)]);
    }
}

void choose_reward(Game& game, int owner, int choice) {
    if (game.run.phase != RunPhase::Reward || owner < 0 || owner >= 4 ||
        choice < 0 || choice >= 3 || game.run.chosen[static_cast<std::size_t>(owner)]) return;
    Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
    if (player == nullptr) return;
    const Reward reward = game.run.offers[static_cast<std::size_t>(owner)]
                                             [static_cast<std::size_t>(choice)];
    switch (reward.kind) {
    case RewardKind::Item:
        if (!insert_item(player->inventory, make_item(reward.item, reward.amount))) return;
        break;
    case RewardKind::Artifact:
        if (!has_artifact(*player, reward.artifact)) {
            player->artifacts |= 1U << static_cast<unsigned int>(reward.artifact);
            if (reward.artifact == ArtifactKind::FleetFeet)
                player->move_interval = std::max(3, player->move_interval - 2);
        }
        break;
    case RewardKind::Health:
        player->max_health += reward.amount;
        player->health = std::min(player->max_health, player->health + reward.amount);
        break;
    case RewardKind::Speed:
        player->move_interval = std::max(3, player->move_interval - reward.amount);
        break;
    }
    game.run.chosen[static_cast<std::size_t>(owner)] = true;
    advance_run(game);
}

void buy_shop_item(Game& game, int owner, int choice) {
    if (game.run.phase != RunPhase::Shop || owner < 0 || owner >= 4 ||
        choice < 0 || choice >= 3) return;
    const ItemKind kind = game.run.shop_stock[static_cast<std::size_t>(choice)];
    if (kind == ItemKind::None || game.run.coins[static_cast<std::size_t>(owner)] < price(kind))
        return;
    Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
    if (player == nullptr || !insert_item(player->inventory, make_item(kind))) return;
    game.run.coins[static_cast<std::size_t>(owner)] -= price(kind);
    game.run.shop_stock[static_cast<std::size_t>(choice)] = ItemKind::None;
}

void advance_run(Game& game) {
    if (game.run.phase == RunPhase::Reward) {
        for (bool chosen : game.run.chosen) if (!chosen) return;
        if (game.run.floor % 2 == 0) {
            game.run.phase = RunPhase::Shop;
            game.run.shop_ready.fill(false);
            game.run.shop_stock = {ItemKind::Bandage, ItemKind::Buckler,
                                   game.run.floor > 2 ? ItemKind::RocketLauncher : ItemKind::Pistol};
            return;
        }
        ready_next_floor(game);
    } else if (game.run.phase == RunPhase::Shop) {
        for (std::size_t owner = 0; owner < 4; ++owner) {
            const Entity* player = get_entity(game, game.players[owner]);
            if (player != nullptr && player->health > 0 && !game.run.shop_ready[owner]) return;
        }
        ready_next_floor(game);
    }
}

int shop_price(ItemKind kind) { return price(kind); }
