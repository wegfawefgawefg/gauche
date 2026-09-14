#include "game.hpp"
#include "projectiles/recoverable.hpp"
#include "items/campfire.hpp"
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

ItemAttribute rare_attribute(Game& game, ItemKind kind) {
    if (random_u32(game) % 7 != 0) return ItemAttribute::None;
    constexpr ItemAttribute choices[]{ItemAttribute::Strong, ItemAttribute::Agile,
        ItemAttribute::Durable, ItemAttribute::Fragile, ItemAttribute::Heavy,
        ItemAttribute::Big, ItemAttribute::Long, ItemAttribute::Piercing,
        ItemAttribute::Restorative};
    std::array<ItemAttribute, 9> eligible{};
    int count = 0;
    for (ItemAttribute choice : choices)
        if (item_accepts_attribute(kind, choice))
            eligible[static_cast<std::size_t>(count++)] = choice;
    return count == 0 ? ItemAttribute::None :
        eligible[random_u32(game) % static_cast<std::uint32_t>(count)];
}

Reward random_reward(Game& game, int category) {
    if (category == 0 && ice_floor(game.run.floor) && random_u32(game) % 4 == 0) {
        constexpr ItemKind cold_tools[]{ItemKind::GritPouch, ItemKind::IceNeedle, ItemKind::AirBladder, ItemKind::ColdFlask, ItemKind::HeatCapsule, ItemKind::WoolWrap, ItemKind::HotBroth, ItemKind::IcePoultice, ItemKind::Chisel, ItemKind::IceBrick, ItemKind::EelBattery, ItemKind::SnowScoop, ItemKind::Snowball, ItemKind::LensCarbine, ItemKind::MirrorShard, ItemKind::CrystalLens, ItemKind::PrismBomb, ItemKind::BlackFelt};
        const ItemKind kind = cold_tools[random_u32(game) % std::size(cold_tools)];
        return {RewardKind::Item, kind, ArtifactKind::None,
                kind == ItemKind::IceNeedle ? 3 : 1, rare_attribute(game, kind)};
    }
    if (category == 0 && game.run.floor <= 4 && random_u32(game) % 2 == 0) {
        constexpr ItemKind finds[]{ItemKind::Hatchet, ItemKind::HuntingSpear,
            ItemKind::Crossbow, ItemKind::Blunderbuss, ItemKind::WoodenMaul,
            ItemKind::Rake, ItemKind::FlintKnife, ItemKind::ThrowingRock, ItemKind::Torch,
            ItemKind::Lighter, ItemKind::OilFlask, ItemKind::SapJar, ItemKind::WaterFlask,
            ItemKind::SmokePot, ItemKind::MushroomSpores, ItemKind::HoneyPot, ItemKind::DiggingClaws,
            ItemKind::ResinGlue, ItemKind::SeedBag, ItemKind::LanternSeed,
            ItemKind::HerbBag, ItemKind::Splint, ItemKind::BitterRoot, ItemKind::Chili, ItemKind::FungalBread, ItemKind::BirdSeed, ItemKind::ThornCaltrops, ItemKind::HuntingHorn, ItemKind::RopeHook, ItemKind::RootDrill, ItemKind::SwapSeed, ItemKind::Boomerang, ItemKind::RopeSnare, ItemKind::SpringTrap, ItemKind::AcornMine, ItemKind::StinkBomb, ItemKind::RottenFruit, ItemKind::PitchBomb, ItemKind::ShieldLantern, ItemKind::ReflectingPan, ItemKind::Scarecrow, ItemKind::StrawDecoy, ItemKind::WolfWhistle, ItemKind::ThunderAcorn, ItemKind::PocketDoor, ItemKind::HandBell, ItemKind::Firecracker, ItemKind::ThrowingNet, ItemKind::StickyBoots, ItemKind::RabbitCharm};
        const ItemKind kind = finds[random_u32(game) % std::size(finds)];
        return {RewardKind::Item, kind, ArtifactKind::None,
                kind == ItemKind::ThrowingRock ? 3 : 1, rare_attribute(game, kind)};
    }
    if (category == 0) {
        constexpr std::array<ItemKind, 11> items{
            ItemKind::Pistol, ItemKind::Bow, ItemKind::Musket, ItemKind::Buckler,
            ItemKind::Bomb, ItemKind::Medkit, ItemKind::RocketLauncher,
            ItemKind::SleepMeds, ItemKind::Shotgun, ItemKind::SMG,
            ItemKind::Pickaxe};
        const ItemKind kind = items[random_u32(game) % items.size()];
        return {RewardKind::Item, kind, ArtifactKind::None,
                kind == ItemKind::Bomb ? 3 :
                (kind == ItemKind::Medkit || kind == ItemKind::SleepMeds ? 2 : 1),
                rare_attribute(game, kind)};
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
    if (game.run.floor > 12) {
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
    game.run.online[0] = true;
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
            unlock_exit_light(game);
            emit_sound(game, SoundId::Confirm, fixture.cell, false);
            remove_entity(game, {static_cast<int>(&fixture - game.entities.data()),
                                 fixture.generation});
            return true;
        }
        if (fixture.kind == EntityKind::Encounter) return request_encounter(game, fixture);
        if (fixture.kind == EntityKind::Switch && !fixture.fixture_open) {
            fixture.fixture_open = true;
            game.run.has_key = true;
            unlock_exit_light(game);
            emit_sound(game, SoundId::SuperConfirm, fixture.cell, false);
            return true;
        }
        if (fixture.kind == EntityKind::Campfire && use_campfire(game, *player, fixture))
            return true;
        if (fixture.kind == EntityKind::Door && game.run.has_key) {
            fixture.fixture_open = true;
            fixture.impassable = false;
            fixture.hard_blocker = false;
            emit_sound(game, SoundId::BlockLand, fixture.cell);
            return true;
        }
        if (fixture.kind == EntityKind::Exit && game.run.phase == RunPhase::Playing) {
            if (!encounter_released(game, fixture.entity_a)) return false;
            for (std::size_t member_owner = 0; member_owner < 4; ++member_owner) {
                if (!game.run.online[member_owner]) continue;
                const Entity* member = get_entity(game, game.players[member_owner]);
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
    finish_recoverables(game);
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

namespace {

bool grant_reward(Entity& player, Reward reward) {
    switch (reward.kind) {
    case RewardKind::Item:
        if (!insert_item(player.inventory, reward_item(reward))) return false;
        break;
    case RewardKind::Artifact:
        if (!has_artifact(player, reward.artifact)) {
            player.artifacts |= 1U << static_cast<unsigned int>(reward.artifact);
            if (reward.artifact == ArtifactKind::FleetFeet)
                player.move_interval = std::max(3, player.move_interval - 2);
        }
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

void choose_reward(Game& game, int owner, int choice) {
    if (game.run.phase != RunPhase::Reward || owner < 0 || owner >= 4 ||
        choice < 0 || choice >= 3 || game.run.chosen[static_cast<std::size_t>(owner)]) return;
    Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
    if (player == nullptr || !grant_reward(*player,
        game.run.offers[static_cast<std::size_t>(owner)][static_cast<std::size_t>(choice)])) return;
    game.run.chosen[static_cast<std::size_t>(owner)] = true;
    advance_run(game);
}

void choose_pending_reward(Game& game, int owner, int choice) {
    if (owner < 0 || owner >= 4 || choice < 0 || choice >= 3 ||
        game.run.pending_count[static_cast<std::size_t>(owner)] == 0) return;
    Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
    if (player == nullptr || player->health <= 0 || !grant_reward(*player,
        game.run.pending_offers[static_cast<std::size_t>(owner)][0]
                               [static_cast<std::size_t>(choice)])) return;
    const std::size_t index = static_cast<std::size_t>(owner);
    for (int pending = 1; pending < game.run.pending_count[index]; ++pending)
        game.run.pending_offers[index][static_cast<std::size_t>(pending - 1)] =
            game.run.pending_offers[index][static_cast<std::size_t>(pending)];
    game.run.pending_offers[index][static_cast<std::size_t>(--game.run.pending_count[index])] = {};
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
        for (std::size_t owner = 0; owner < 4; ++owner)
            if (game.run.online[owner] && !game.run.chosen[owner]) return;
        for (std::size_t owner = 0; owner < 4; ++owner) {
            if (game.run.online[owner] || game.run.chosen[owner] ||
                game.run.pending_count[owner] >= 12) continue;
            game.run.pending_offers[owner]
                [static_cast<std::size_t>(game.run.pending_count[owner]++)] = game.run.offers[owner];
            game.run.chosen[owner] = true;
        }
        if (game.run.floor % 2 == 0) {
            game.run.phase = RunPhase::Shop;
            game.run.shop_ready.fill(false);
            game.run.shop_stock = {ItemKind::Bandage,
                                   game.run.floor > 4 ? ItemKind::Mine : ItemKind::Buckler,
                                   game.run.floor > 8 ? ItemKind::RocketLauncher :
                                   (game.run.floor > 2 ? ItemKind::Shotgun : ItemKind::Pistol)};
            if (ice_floor(game.run.floor)) {
                constexpr ItemKind cold_tools[]{ItemKind::GritPouch, ItemKind::IceNeedle, ItemKind::AirBladder, ItemKind::ColdFlask, ItemKind::HeatCapsule, ItemKind::WoolWrap, ItemKind::HotBroth, ItemKind::IcePoultice, ItemKind::Chisel, ItemKind::IceBrick, ItemKind::EelBattery, ItemKind::SnowScoop, ItemKind::Snowball, ItemKind::LensCarbine, ItemKind::MirrorShard, ItemKind::CrystalLens, ItemKind::PrismBomb, ItemKind::BlackFelt};
                game.run.shop_stock[1] = cold_tools[random_u32(game) % std::size(cold_tools)];
            }
            if (game.run.floor <= 4) {
                constexpr ItemKind tools[]{ItemKind::Hatchet, ItemKind::WoodenMaul,
                    ItemKind::HuntingSpear, ItemKind::FlintKnife, ItemKind::Torch, ItemKind::Lighter,
                    ItemKind::OilFlask, ItemKind::SapJar, ItemKind::WaterFlask, ItemKind::SmokePot,
                    ItemKind::HoneyPot, ItemKind::MushroomSpores, ItemKind::DiggingClaws,
                    ItemKind::ResinGlue, ItemKind::SeedBag, ItemKind::LanternSeed,
                    ItemKind::HerbBag, ItemKind::Splint, ItemKind::BitterRoot, ItemKind::Chili, ItemKind::FungalBread, ItemKind::BirdSeed, ItemKind::ThornCaltrops, ItemKind::HuntingHorn, ItemKind::RopeHook, ItemKind::RootDrill, ItemKind::SwapSeed, ItemKind::Boomerang, ItemKind::RopeSnare, ItemKind::SpringTrap, ItemKind::AcornMine, ItemKind::StinkBomb, ItemKind::RottenFruit, ItemKind::PitchBomb, ItemKind::ShieldLantern, ItemKind::ReflectingPan, ItemKind::Scarecrow, ItemKind::StrawDecoy, ItemKind::WolfWhistle, ItemKind::ThunderAcorn, ItemKind::PocketDoor, ItemKind::HandBell, ItemKind::Firecracker, ItemKind::ThrowingNet, ItemKind::StickyBoots, ItemKind::RabbitCharm};
                game.run.shop_stock[1] = tools[random_u32(game) % std::size(tools)];
                game.run.shop_stock[2] = random_u32(game) % 2 == 0 ?
                    ItemKind::Crossbow : ItemKind::Blunderbuss;
            }
            return;
        }
        ready_next_floor(game);
    } else if (game.run.phase == RunPhase::Shop) {
        for (std::size_t owner = 0; owner < 4; ++owner) {
            const Entity* player = get_entity(game, game.players[owner]);
            if (game.run.online[owner] && player != nullptr && player->health > 0 &&
                !game.run.shop_ready[owner]) return;
        }
        ready_next_floor(game);
    }
}

int shop_price(ItemKind kind) { return price(kind); }
