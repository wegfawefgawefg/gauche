#include "net_codec.hpp"
#include "net/entity_codec.hpp"
#include "props/cloth.hpp"
#include "props/candle.hpp"
#include "props/stove.hpp"
#include "projectiles/projectile.hpp"

// SNAPSHOT: World and run fields precede entities and cross-entity reservations.
std::vector<std::uint8_t> encode_game(const Game& game) {
    PacketWriter writer;
    writer.u32(36);
    writer.u64(game.rng); writer.u64(game.tick);
    writer.u8(static_cast<std::uint8_t>(game.started));
    writer.u8(static_cast<std::uint8_t>(game.game_over));
    writer.i32(game.stage.width); writer.i32(game.stage.height);
    for (const Tile& tile : game.stage.tiles) {
        writer.u8(static_cast<std::uint8_t>(tile.kind));
        writer.u16(tile.hp); writer.u8(tile.water_phase);
        writer.u16(tile.max_hp);
        writer.u8(static_cast<std::uint8_t>(tile.break_rule));
        writer.u8(tile.required_dig_power);
        writer.u8(static_cast<std::uint8_t>(tile.material));
        writer.u8(static_cast<std::uint8_t>(tile.thaw_kind));
        writer.u16(tile.freeze_ticks);
        writer.u8(static_cast<std::uint8_t>(tile.surface.liquid));
        writer.u8(tile.surface.gritted ? 1 : 0);
        for (auto ticks : {tile.surface.liquid_ticks, tile.surface.fire_ticks, tile.surface.smoke_ticks, tile.surface.sleep_ticks, tile.surface.scent_ticks, tile.surface.warmth_ticks, tile.surface.whiteout_ticks}) writer.u16(ticks);
        writer.u8(static_cast<std::uint8_t>(tile.prop.kind));
        writer.u8(tile.prop.hp); writer.u8(tile.prop.variant);
        writer.u8(static_cast<std::uint8_t>(tile.prop.broken));
        writer.u16(tile.prop.growth_ticks);
        writer.u8(tile.prop.covered ? 1 : 0);
    }
    const Run& run = game.run;
    writer.u8(static_cast<std::uint8_t>(run.phase));
    writer.i32(run.floor); writer.u64(run.seed);
    writer.u8(static_cast<std::uint8_t>(run.death_policy));
    writer.u8(static_cast<std::uint8_t>(run.has_key));
    writer.u8(static_cast<std::uint8_t>(run.objective));
    writer.cell(run.spawn); writer.cell(run.exit);
    writer.i32(run.roof_light_count);
    for (const StageLight& light : run.roof_lights) {
        writer.cell(light.cell);
        writer.light(light.light);
    }
    for (std::size_t owner = 0; owner < 4; ++owner) {
        writer.i32(game.players[owner].slot);
        writer.u32(game.players[owner].generation);
        writer.i32(run.coins[owner]);
        writer.u8(static_cast<std::uint8_t>(run.chosen[owner]));
        writer.u8(static_cast<std::uint8_t>(run.shop_ready[owner]));
        writer.u8(static_cast<std::uint8_t>(run.online[owner]));
        writer.i32(run.pending_count[owner]);
        for (const Reward& reward : run.offers[owner]) {
            writer.u8(static_cast<std::uint8_t>(reward.kind));
            writer.u8(static_cast<std::uint8_t>(reward.item));
            writer.u8(static_cast<std::uint8_t>(reward.artifact));
            writer.i32(reward.amount);
            writer.u8(static_cast<std::uint8_t>(reward.attribute));
        }
        for (const auto& offer : run.pending_offers[owner]) {
            for (const Reward& reward : offer) {
                writer.u8(static_cast<std::uint8_t>(reward.kind));
                writer.u8(static_cast<std::uint8_t>(reward.item));
                writer.u8(static_cast<std::uint8_t>(reward.artifact));
                writer.i32(reward.amount);
                writer.u8(static_cast<std::uint8_t>(reward.attribute));
            }
        }
    }
    for (ItemKind item : run.shop_stock) writer.u8(static_cast<std::uint8_t>(item));
    for (const Entity& entity : game.entities) write_entity(writer, entity);
    writer.u32(static_cast<std::uint32_t>(game.flight_contacts.size()));
    for (const FlightContact& hit : game.flight_contacts)
        for (Handle handle : {hit.projectile, hit.victim}) {
            writer.i32(handle.slot); writer.u32(handle.generation);
        }
    return writer.bytes;
}

bool decode_game(std::span<const std::uint8_t> bytes, Game& game, std::string& error) {
    PacketReader reader{bytes};
    if (reader.u32() != 36) { error = "Snapshot version mismatch"; return false; }
    Game result;
    result.rng = reader.u64(); result.tick = reader.u64();
    result.started = reader.u8() != 0;
    result.game_over = reader.u8() != 0;
    result.stage.width = reader.i32(); result.stage.height = reader.i32();
    if (result.stage.width < 0 || result.stage.height < 0 ||
        result.stage.width > 512 || result.stage.height > 512 ||
        static_cast<std::int64_t>(result.stage.width) * result.stage.height > 262144) {
        error = "Invalid snapshot dimensions";
        return false;
    }
    const auto tile_count = static_cast<std::size_t>(result.stage.width * result.stage.height);
    result.stage.tiles.resize(tile_count);
    for (Tile& tile : result.stage.tiles) {
        const std::uint8_t kind = reader.u8();
        if (kind >= static_cast<std::uint8_t>(TileKind::Count)) reader.okay = false;
        tile.kind = static_cast<TileKind>(kind);
        tile.hp = reader.u16(); tile.water_phase = reader.u8();
        tile.max_hp = reader.u16();
        tile.break_rule = static_cast<BreakRule>(reader.u8());
        tile.required_dig_power = reader.u8();
        tile.material = static_cast<TileMaterial>(reader.u8());
        if (tile.material >= TileMaterial::Count) reader.okay = false;
        tile.thaw_kind = static_cast<TileKind>(reader.u8());
        tile.freeze_ticks = reader.u16();
        if (tile.freeze_ticks > 480 || (tile.freeze_ticks == 0 ? tile.thaw_kind != TileKind::Empty :
            (tile.thaw_kind != TileKind::ShallowWater && tile.thaw_kind != TileKind::Spring &&
             tile.thaw_kind != TileKind::IceHole))) reader.okay = false;
        tile.surface.liquid = static_cast<LiquidKind>(reader.u8());
        const auto gritted = reader.u8();
        if (gritted > 1) reader.okay = false;
        tile.surface.gritted = gritted != 0;
        tile.surface.liquid_ticks = reader.u16(); tile.surface.fire_ticks = reader.u16();
        tile.surface.smoke_ticks = reader.u16(); tile.surface.sleep_ticks = reader.u16(); tile.surface.scent_ticks = reader.u16();
        tile.surface.warmth_ticks = reader.u16(); tile.surface.whiteout_ticks = reader.u16();
        if (tile.surface.warmth_ticks > 240 || tile.surface.scent_ticks > 600 || tile.surface.whiteout_ticks > 360) reader.okay = false;
        if (tile.surface.liquid >= LiquidKind::Count) reader.okay = false;
        tile.prop.kind = static_cast<PropKind>(reader.u8());
        tile.prop.hp = reader.u8(); tile.prop.variant = reader.u8();
        tile.prop.broken = reader.u8() != 0;
        tile.prop.growth_ticks = reader.u16();
        const auto covered = reader.u8();
        tile.prop.covered = covered != 0;
        if (covered > 1 || (tile.prop.covered && !coverable_prop(tile.prop))) reader.okay = false;
        if (tile.prop.kind >= PropKind::Count ||
            tile.prop.hp > prop_max_health(tile.prop) ||
            (tile.prop.broken && (tile.prop.hp != 0 || tile.prop.growth_ticks != 0)) ||
            tile.prop.growth_ticks > (tile.prop.kind == PropKind::GroundingSpike ? 180 : tile.prop.kind == PropKind::Stove ? stove_fuel_limit : tile.prop.kind == PropKind::Candle ? candle_fuel_ticks : tile.prop.kind == PropKind::IceBlock ? 600 : tile.prop.kind == PropKind::AlarmClock ? 480 : 180) ||
            (tile.prop.kind != PropKind::GroundingSpike && tile.prop.kind != PropKind::Stove && tile.prop.kind != PropKind::Candle && tile.prop.kind != PropKind::Shoot && tile.prop.kind != PropKind::IceBlock && tile.prop.kind != PropKind::AlarmClock && tile.prop.growth_ticks != 0)) reader.okay = false;
        if (tile.prop.kind == PropKind::GroundingSpike && (tile.prop.variant > 2 ||
            (!tile.prop.broken && tile.prop.hp == 0) ||
            (tile.prop.variant != 1 && tile.prop.growth_ticks != 0))) reader.okay = false;
        if (tile.prop.kind == PropKind::CopperWire && (tile.prop.variant != 0 ||
            (!tile.prop.broken && tile.prop.hp == 0))) reader.okay = false;
        if (tile.prop.kind == PropKind::Candle && (tile.prop.variant > 3 || (!tile.prop.broken && tile.prop.hp == 0))) reader.okay = false;
        if (tile.prop.kind == PropKind::SpiderStrand && (tile.prop.variant > 1 || tile.prop.hp != 1 || tile.prop.broken)) reader.okay = false;
        if ((tile.prop.kind == PropKind::Stove || tile.prop.kind == PropKind::AlarmClock) && (tile.prop.variant > 1 || (!tile.prop.broken && tile.prop.hp == 0))) reader.okay = false;
        if (tile.hp > tile.max_hp || tile.break_rule > BreakRule::DigRequired)
            reader.okay = false;
    }
    Run& run = result.run;
    const std::uint8_t phase = reader.u8();
    if (phase > static_cast<std::uint8_t>(RunPhase::Won)) reader.okay = false;
    run.phase = static_cast<RunPhase>(phase);
    run.floor = reader.i32(); run.seed = reader.u64();
    const std::uint8_t death_policy = reader.u8();
    if (death_policy > static_cast<std::uint8_t>(DeathPolicy::NextFloor)) reader.okay = false;
    run.death_policy = static_cast<DeathPolicy>(death_policy);
    run.has_key = reader.u8() != 0;
    const std::uint8_t objective = reader.u8();
    if (objective > static_cast<std::uint8_t>(ObjectiveKind::Switch)) reader.okay = false;
    run.objective = static_cast<ObjectiveKind>(objective);
    run.spawn = reader.cell(); run.exit = reader.cell();
    run.roof_light_count = reader.i32();
    if (run.roof_light_count < 0 || run.roof_light_count > 16) reader.okay = false;
    for (StageLight& light : run.roof_lights) {
        light.cell = reader.cell();
        light.light = reader.light();
    }
    for (std::size_t owner = 0; owner < 4; ++owner) {
        result.players[owner].slot = reader.i32();
        result.players[owner].generation = reader.u32();
        if (result.players[owner].slot < -1 || result.players[owner].slot >= max_entities)
            reader.okay = false;
        run.coins[owner] = reader.i32();
        run.chosen[owner] = reader.u8() != 0;
        run.shop_ready[owner] = reader.u8() != 0;
        run.online[owner] = reader.u8() != 0;
        run.pending_count[owner] = reader.i32();
        if (run.pending_count[owner] < 0 || run.pending_count[owner] > 12)
            reader.okay = false;
        for (Reward& reward : run.offers[owner]) {
            reward.kind = static_cast<RewardKind>(reader.u8());
            reward.item = static_cast<ItemKind>(reader.u8());
            reward.artifact = static_cast<ArtifactKind>(reader.u8());
            reward.amount = reader.i32();
            reward.attribute = static_cast<ItemAttribute>(reader.u8());
            if (reward.kind > RewardKind::Speed || reward.item >= ItemKind::Count ||
                reward.artifact > ArtifactKind::FleetFeet || reward.amount < 0 ||
                reward.attribute > ItemAttribute::Restorative)
                reader.okay = false;
        }
        for (auto& offer : run.pending_offers[owner]) {
            for (Reward& reward : offer) {
                reward.kind = static_cast<RewardKind>(reader.u8());
                reward.item = static_cast<ItemKind>(reader.u8());
                reward.artifact = static_cast<ArtifactKind>(reader.u8());
                reward.amount = reader.i32();
                reward.attribute = static_cast<ItemAttribute>(reader.u8());
                if (reward.kind > RewardKind::Speed || reward.item >= ItemKind::Count ||
                    reward.artifact > ArtifactKind::FleetFeet || reward.amount < 0 ||
                    reward.attribute > ItemAttribute::Restorative)
                    reader.okay = false;
            }
        }
    }
    for (ItemKind& item : run.shop_stock) {
        item = static_cast<ItemKind>(reader.u8());
        if (item >= ItemKind::Count) reader.okay = false;
    }
    for (Entity& entity : result.entities) entity = read_entity(reader);
    const std::uint32_t contacts = reader.u32();
    if (!reader.okay || contacts > max_entities * max_entities ||
        contacts > (bytes.size() - reader.position) / 16) {
        error = "Invalid projectile contacts"; return false;
    }
    for (std::uint32_t index = 0; index < contacts; ++index) {
        FlightContact hit{{reader.i32(), reader.u32()}, {reader.i32(), reader.u32()}};
        const Entity* shot = get_entity(result, hit.projectile);
        if (shot == nullptr || shot->kind != EntityKind::Projectile ||
            (shot->label_a != static_cast<int>(ProjectileKind::Rock) &&
             shot->label_a != static_cast<int>(ProjectileKind::Boomerang)) ||
            hit.victim.slot < 0 || hit.victim.slot >= max_entities) reader.okay = false;
        result.flight_contacts.push_back(hit);
    }
    // RESERVATIONS: A carried placeholder must name its owner's physical projectile.
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& actor = result.entities[static_cast<std::size_t>(slot)];
        if (actor.kind == EntityKind::None) continue;
        for (const Item& item : actor.inventory.slots) {
            if (item.flight.slot < 0) continue;
            const Entity* shot = get_entity(result, item.flight);
            const Handle owner{slot,actor.generation};
            const bool valid = shot && shot->kind == EntityKind::Projectile &&
                (item.kind == ItemKind::HarpoonGun ?
                    shot->label_a == static_cast<int>(ProjectileKind::Harpoon) && shot->entity_a == owner :
                    shot->label_a == static_cast<int>(ProjectileKind::Boomerang) && shot->entity_b == owner);
            if (!valid) reader.okay = false;
        }
    }
    if (!reader.finished()) { error = "Invalid or truncated snapshot"; return false; }
    for (Handle handle : result.players) {
        if (handle.slot >= 0 && get_entity(result, handle) == nullptr) {
            error = "Snapshot player handle is stale";
            return false;
        }
    }
    result.sound_count = 0;
    game = std::move(result);
    return true;
}
