#include "artifacts/powers.hpp"
#include "artifacts/catalog.hpp"
#include "world/reactor.hpp"
#include "items/supply.hpp"
#include "run/offers.hpp"
#include "game.hpp"
#include "projectiles/recoverable.hpp"
#include "items/campfire.hpp"
#include "items/cooking.hpp"
#include "items/catalog.hpp"
#include "world/encounter.hpp"
#include "world/ice_terrain.hpp"
#include "item_attribute.hpp"

#include <algorithm>
#include <array>
#include <deque>
#include <utility>
#include <vector>

namespace {

void unlock_exit_light(Game& game) {
    for (Entity& entity : game.entities)
        if (entity.kind == EntityKind::Exit)
            entity.light.color = {84, 255, 135};
}

std::array<Reward,3> roll_rewards(Game& game,const Entity& player) {
    std::array<Reward,3> offers{};
    for (int slot=0;slot<3;++slot) {
        std::vector<ArtifactKind> choices;
        for (const auto kind:artifact_kinds) {
            if (!artifact_eligible(player,kind)) continue;
            bool duplicate=false;
            for (int i=0;i<slot;++i) duplicate|=offers[static_cast<std::size_t>(i)].artifact==kind;
            if (duplicate) continue;
            const int weight=kind==ArtifactKind::GodHand ? 1 : artifact_stackable(kind) ? 12 : 4;
            for (int i=0;i<weight;++i) choices.push_back(kind);
        }
        if (!choices.empty()) offers[static_cast<std::size_t>(slot)]={RewardKind::Artifact,ItemKind::None,
            choices[random_u32(game)%choices.size()],1};
        else offers[static_cast<std::size_t>(slot)]={RewardKind::Health,ItemKind::None,ArtifactKind::None,20};
    }
    return offers;
}

int price(ItemKind kind) {
    if (const RegionalItem* spec = regional_item(kind)) return spec->price;
    switch (kind) {
    case ItemKind::Bandage: return 12;
    case ItemKind::Buckler: return 35;
    case ItemKind::Pistol: return 45;
    case ItemKind::Musket: return 50;
    case ItemKind::RocketLauncher: return 90;
    case ItemKind::Shotgun: return 70;
    case ItemKind::SMG: return 75;
    case ItemKind::BearTrap: return 18;
    case ItemKind::Mine: return 28;
    case ItemKind::Pickaxe: return 25;
    default: return 25;
    }
}

void ready_next_floor(Game& game) {
    ++game.run.floor;
    if (game.run.floor > run_floor_count) {
        game.run.phase = RunPhase::Won;
        return;
    }
    generate_world_floor(game);
}

} // namespace

Item reward_item(Reward reward) {
    return make_item(reward.item, reward.amount, reward.attribute);
}

void start_run(Game& game, std::uint64_t seed) {
    game = {};
    game.rng = seed == 0 ? 1 : seed;
    game.run.seed = game.rng;
    game.run.floor = 1;
    game.run.phase = RunPhase::Playing;
    player_state(game, 0).online = true;
    generate_world_floor(game);
}

bool interact_with_fixture(Game& game, int owner, Cell target, bool held_use) {
    if (owner < 0 || !has_player(game, owner)) return false;
    Entity* player = get_entity(game, player_state(game, owner).controlled);
    if (player == nullptr || player->health <= 0 || distance(player->cell, target) > 1)
        return false;
    for (Entity& fixture : game.entities) {
        if (fixture.cell != target) continue;
        if (fixture.kind == EntityKind::Key && player->cell == target) {
            game.run.has_key = true;
            unlock_exit_light(game);
            emit_sound(game, SoundId::Confirm, fixture.cell, false);
            remove_entity(game, {static_cast<int>(&fixture - game.entities.data()),
                                 fixture.generation});
            return true;
        }
        if (fixture.kind == EntityKind::Encounter) return request_encounter(game, fixture);
        if (fixture.kind == EntityKind::Switch && !fixture.fixture_open) {
            if (fixture.sprite==Sprite::ReactorCore)
                return !held_use && activate_reactor(game,fixture);
            fixture.fixture_open = true;
            game.run.has_key = true;
            unlock_exit_light(game);
            emit_sound(game, SoundId::SuperConfirm, fixture.cell, false);
            return true;
        }
        if (fixture.kind == EntityKind::Campfire) {
            const auto held = player->inventory.held()->kind;
            // Explicit interaction may cook from the pack; using a flask, gun or
            // other utility keeps that item's action even when meat is carried.
            const bool cooking_hand = item_is_melee(held) || held == ItemKind::RawMeat || held == ItemKind::Egg;
            if ((!held_use || cooking_hand) && use_campfire(game, *player, fixture,held_use)) return true;
        }
        if (fixture.kind == EntityKind::Door && game.run.has_key) {
            fixture.fixture_open = true;
            fixture.impassable = false;
            fixture.hard_blocker = false;
            emit_sound(game, SoundId::BlockLand, fixture.cell);
            return true;
        }
        if (fixture.kind == EntityKind::Exit && game.run.phase == RunPhase::Playing) {
            if (game.run.layout==FloorLayout::LastShift && !game.run.has_key) return false;
            if (!encounter_released(game, fixture.entity_a)) return false;
            for (const auto& [member_owner, participant] : game.players) {
                if (!player_state(game, member_owner).online) continue;
                const Entity* member = get_entity(game, player_state(game, member_owner).controlled);
                if (member != nullptr && member->health > 0 &&
                    distance(member->cell, fixture.cell) > 1) return false;
            }
            finish_floor(game);
            return true;
        }
    }
    const auto held=player->inventory.held()->kind;
    if (!held_use || item_is_melee(held) || cooked_item(held)!=ItemKind::None)
        return use_cooking_fire(game,*player,target,held_use);
    return false;
}

void finish_floor(Game& game) {
    finish_recoverables(game);
    game.run.phase = RunPhase::Reward;
    emit_sound(game, SoundId::LevelWin, game.run.exit, false);
    for (auto& [id, member] : game.players) member.chosen = false;
    for (const auto& [owner, participant] : game.players) {
        const Entity* player = get_entity(game, player_state(game, owner).controlled);
        if (player == nullptr || player->health <= 0) {
            player_state(game, owner).chosen = true;
            continue;
        }
        auto& offers = player_state(game, owner).offers;
        offers=roll_rewards(game,*player);
    }
}

namespace {

bool grant_reward(Game& game, Entity& player, Reward reward, int replace_slot, ItemKind expected) {
    if (replace_slot>=0 && reward.kind!=RewardKind::Item) return false;
    switch (reward.kind) {
    case RewardKind::Item:
        if (!accept_offer_item(game,player,reward_item(reward),replace_slot,expected)) return false;
        break;
    case RewardKind::Artifact:
        if (!artifact_eligible(player,reward.artifact)) return false;
        grant_artifact(player,reward.artifact);
        break;
    case RewardKind::Health:
        player.max_health += reward.amount;
        player.health = std::min(player.max_health, player.health + reward.amount);
        break;
    case RewardKind::Speed:
        player.move_interval = std::max(3, player.move_interval - reward.amount);
        break;
    }
    return true;
}

} // namespace

void choose_reward(Game& game, int owner, int choice, int replace_slot, ItemKind expected) {
    if (game.run.phase != RunPhase::Reward || owner < 0 || !has_player(game, owner) ||
        choice < 0 || choice >= 3 || player_state(game, owner).chosen) return;
    Entity* player = get_entity(game, player_state(game, owner).controlled);
    if (player == nullptr || player->health<=0 || !grant_reward(game,*player,
        player_state(game, owner).offers[static_cast<std::size_t>(choice)],replace_slot,expected)) return;
    player_state(game, owner).chosen = true;
    emit_sound(game,SoundId::Confirm,player->cell,false);
    advance_run(game);
}

void choose_pending_reward(Game& game, int owner, int choice, int replace_slot, ItemKind expected) {
    if (owner < 0 || !has_player(game, owner) || choice < 0 || choice >= 3 ||
        player_state(game, owner).pending_count == 0) return;
    Entity* player = get_entity(game, player_state(game, owner).controlled);
    if (player == nullptr || player->health <= 0 || !grant_reward(game,*player,
        player_state(game, owner).pending_offers[0]
                               [static_cast<std::size_t>(choice)],replace_slot,expected)) return;
    const PlayerId index = owner;
    for (int pending = 1; pending < player_state(game, index).pending_count; ++pending)
        player_state(game, index).pending_offers[static_cast<std::size_t>(pending - 1)] =
            player_state(game, index).pending_offers[static_cast<std::size_t>(pending)];
    player_state(game, index).pending_offers[static_cast<std::size_t>(--player_state(game, index).pending_count)] = {};
    emit_sound(game,SoundId::Confirm,player->cell,false);
}

void buy_shop_item(Game& game, int owner, int choice, int replace_slot, ItemKind expected) {
    if (game.run.phase != RunPhase::Shop || owner < 0 || !has_player(game, owner) ||
        choice < 0 || choice >= 3) return;
    const ItemKind kind = game.run.shop_stock[static_cast<std::size_t>(choice)];
    if (kind == ItemKind::None || player_state(game, owner).coins < price(kind))
        return;
    Entity* player = get_entity(game, player_state(game, owner).controlled);
    if (player == nullptr || player->health<=0 || player_state(game, owner).shop_ready ||
        !accept_offer_item(game,*player,supply_item(kind),replace_slot,expected)) return;
    player_state(game, owner).coins -= price(kind);
    game.run.shop_stock[static_cast<std::size_t>(choice)] = ItemKind::None;
    emit_sound(game,SoundId::Confirm,player->cell,false);
}

void advance_run(Game& game) {
    if (game.run.phase == RunPhase::Reward) {
        for (const auto& [owner, participant] : game.players)
            if (player_state(game, owner).online && !player_state(game, owner).chosen) return;
        for (const auto& [owner, participant] : game.players) {
            if (player_state(game, owner).online || player_state(game, owner).chosen ||
                player_state(game, owner).pending_count >= 12) continue;
            player_state(game, owner).pending_offers
                [static_cast<std::size_t>(player_state(game, owner).pending_count++)] = player_state(game, owner).offers;
            player_state(game, owner).chosen = true;
        }
        if (game.run.floor % 2 == 0) {
            game.run.phase = RunPhase::Shop;
            for (auto& [id, member] : game.players) member.shop_ready = false;
            game.run.shop_stock[0]=ItemKind::Bandage;
            game.run.shop_stock[1]=roll_item_supply(game,LootSource::Shop,true,ItemKind::Bandage);
            game.run.shop_stock[2]=roll_item_supply(game,LootSource::Weapon,true,game.run.shop_stock[1]);
            return;
        }
        ready_next_floor(game);
    } else if (game.run.phase == RunPhase::Shop) {
        for (const auto& [owner, participant] : game.players) {
            const Entity* player = get_entity(game, player_state(game, owner).controlled);
            if (player_state(game, owner).online && player != nullptr && player->health > 0 &&
                !player_state(game, owner).shop_ready) return;
        }
        ready_next_floor(game);
    }
}

int shop_price(ItemKind kind) { return price(kind); }
