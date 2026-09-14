#include "net_codec.hpp"

#include <algorithm>
#include <limits>

void PacketWriter::u8(std::uint8_t value) { bytes.push_back(value); }
void PacketWriter::u16(std::uint16_t value) {
    for (int bit = 0; bit < 16; bit += 8) u8(static_cast<std::uint8_t>(value >> bit));
}
void PacketWriter::u32(std::uint32_t value) {
    for (int bit = 0; bit < 32; bit += 8) u8(static_cast<std::uint8_t>(value >> bit));
}
void PacketWriter::u64(std::uint64_t value) {
    for (int bit = 0; bit < 64; bit += 8) u8(static_cast<std::uint8_t>(value >> bit));
}
void PacketWriter::i32(int value) { u32(static_cast<std::uint32_t>(value)); }
void PacketWriter::input(const Input& value) {
    i32(value.move.x); i32(value.move.y);
    i32(value.aim.x); i32(value.aim.y);
    u8(static_cast<std::uint8_t>(value.use));
    u8(static_cast<std::uint8_t>(value.pickup));
    u8(static_cast<std::uint8_t>(value.drop));
    u8(static_cast<std::uint8_t>(value.reload));
    u8(static_cast<std::uint8_t>(value.interact));
    u8(static_cast<std::uint8_t>(value.confirm));
    i32(value.select);
}

std::uint8_t PacketReader::u8() {
    if (position >= bytes.size()) { okay = false; return 0; }
    return bytes[position++];
}
std::uint16_t PacketReader::u16() {
    std::uint16_t value = 0;
    for (int bit = 0; bit < 16; bit += 8)
        value = static_cast<std::uint16_t>(value | (static_cast<unsigned int>(u8()) << bit));
    return value;
}
std::uint32_t PacketReader::u32() {
    std::uint32_t value = 0;
    for (int bit = 0; bit < 32; bit += 8)
        value |= static_cast<std::uint32_t>(u8()) << bit;
    return value;
}
std::uint64_t PacketReader::u64() {
    std::uint64_t value = 0;
    for (int bit = 0; bit < 64; bit += 8)
        value |= static_cast<std::uint64_t>(u8()) << bit;
    return value;
}
int PacketReader::i32() {
    return static_cast<int>(static_cast<std::int32_t>(u32()));
}
Input PacketReader::input() {
    Input value;
    value.move = {i32(), i32()};
    value.aim = {i32(), i32()};
    value.use = u8() != 0;
    value.pickup = u8() != 0;
    value.drop = u8() != 0;
    value.reload = u8() != 0;
    value.interact = u8() != 0;
    value.confirm = u8() != 0;
    value.select = i32();
    if (value.move.x < -1 || value.move.x > 1 || value.move.y < -1 ||
        value.move.y > 1 || value.aim.x < -512 || value.aim.x > 512 ||
        value.aim.y < -512 || value.aim.y > 512 ||
        value.select < -1 || value.select >= quick_slots) okay = false;
    return value;
}
bool PacketReader::finished() const { return okay && position == bytes.size(); }

namespace {

void write_cell(PacketWriter& writer, Cell cell) {
    writer.i32(cell.x); writer.i32(cell.y);
}
Cell read_cell(PacketReader& reader) { return {reader.i32(), reader.i32()}; }

void write_light(PacketWriter& writer, LightEmitter light) {
    writer.i32(light.radius);
    writer.i32(light.strength);
    writer.u8(light.color.red); writer.u8(light.color.green); writer.u8(light.color.blue);
}
LightEmitter read_light(PacketReader& reader) {
    LightEmitter light;
    light.radius = reader.i32();
    light.strength = reader.i32();
    light.color = {reader.u8(), reader.u8(), reader.u8()};
    if (light.radius < 0 || light.radius > 128 ||
        light.strength < 0 || light.strength > 10000) reader.okay = false;
    return light;
}

void write_item(PacketWriter& writer, const Item& item) {
    writer.u8(static_cast<std::uint8_t>(item.kind));
    writer.u8(static_cast<std::uint8_t>(item.attribute));
    writer.i32(item.count); writer.i32(item.max_count);
    writer.u8(static_cast<std::uint8_t>(item.consume_on_use));
    writer.i32(item.cooldown); writer.i32(item.loaded);
    writer.i32(item.spare); writer.i32(item.durability);
    writer.i32(item.max_durability); writer.i32(item.uses); writer.i32(item.max_uses);
    writer.u8(static_cast<std::uint8_t>(item.opened));
    write_light(writer, item.light);
    writer.i32(item.dig_power);
}
Item read_item(PacketReader& reader) {
    Item item;
    const std::uint8_t kind = reader.u8();
    if (kind > static_cast<std::uint8_t>(ItemKind::CookedMeat)) reader.okay = false;
    item.kind = static_cast<ItemKind>(kind);
    item.attribute = static_cast<ItemAttribute>(reader.u8());
    item.count = reader.i32(); item.max_count = reader.i32();
    item.consume_on_use = reader.u8() != 0;
    item.cooldown = reader.i32(); item.loaded = reader.i32();
    item.spare = reader.i32(); item.durability = reader.i32();
    item.max_durability = reader.i32(); item.uses = reader.i32();
    item.max_uses = reader.i32(); item.opened = reader.u8() != 0;
    item.light = read_light(reader);
    item.dig_power = reader.i32();
    if (item.dig_power < 0 || item.dig_power > 255) reader.okay = false;
    if (item.count < 0 || item.max_count < item.count || item.cooldown < 0 ||
        item.loaded < 0 || item.spare < 0 ||
        item.durability < 0 || item.max_durability < item.durability ||
        item.uses < 0 || item.max_uses < item.uses ||
        item.attribute > ItemAttribute::Restorative) reader.okay = false;
    return item;
}

void write_entity(PacketWriter& writer, const Entity& entity) {
    writer.u8(static_cast<std::uint8_t>(entity.kind));
    writer.u32(entity.generation);
    if (entity.kind == EntityKind::None) return;
    write_cell(writer, entity.cell); write_cell(writer, entity.facing);
    writer.u16(static_cast<std::uint16_t>(entity.sprite));
    write_light(writer, entity.light);
    writer.u8(entity.self_light.red); writer.u8(entity.self_light.green);
    writer.u8(entity.self_light.blue);
    writer.i32(entity.owner); writer.i32(entity.health); writer.i32(entity.max_health);
    writer.i32(entity.move_wait); writer.i32(entity.move_interval);
    writer.i32(entity.attack_wait); writer.i32(entity.attack_interval);
    writer.i32(entity.use_flash); writer.i32(entity.block_ticks);
    writer.i32(entity.burn_ticks); writer.i32(entity.freeze_ticks);
    writer.i32(entity.scorch_ticks); writer.i32(entity.fire_dim_ticks);
    writer.u8(entity.fire_tramples);
    writer.i32(entity.sleep_ticks); writer.i32(entity.stun_ticks);
    writer.i32(entity.script_tick); writer.u32(entity.artifacts);
    writer.i32(entity.train_cars_left); writer.i32(entity.spawn_wait);
    write_cell(writer, entity.train_origin);
    for (Handle handle : {entity.entity_a, entity.entity_b}) {
        writer.i32(handle.slot); writer.u32(handle.generation);
    }
    write_cell(writer, entity.point_a); write_cell(writer, entity.point_b);
    for (int value : {entity.counter_a, entity.counter_b, entity.label_a, entity.label_b,
                      entity.timer_a, entity.timer_b}) writer.i32(value);
    writer.u64(entity.birth_tick);
    writer.u8(static_cast<std::uint8_t>(entity.impassable));
    writer.u8(static_cast<std::uint8_t>(entity.hard_blocker));
    writer.u8(static_cast<std::uint8_t>(entity.fixture_open));
    writer.i32(entity.inventory.selected);
    for (const Item& item : entity.inventory.slots) write_item(writer, item);
    write_item(writer, entity.ground_item);
}

Entity read_entity(PacketReader& reader) {
    Entity entity;
    const std::uint8_t kind = reader.u8();
    if (kind > static_cast<std::uint8_t>(EntityKind::ZombieStack)) reader.okay = false;
    entity.kind = static_cast<EntityKind>(kind);
    entity.generation = reader.u32();
    if (entity.kind == EntityKind::None) return entity;
    entity.cell = read_cell(reader); entity.facing = read_cell(reader);
    const std::uint16_t sprite = reader.u16();
    if (sprite >= static_cast<std::uint16_t>(Sprite::Count)) reader.okay = false;
    entity.sprite = static_cast<Sprite>(sprite);
    entity.light = read_light(reader);
    entity.self_light = {reader.u8(), reader.u8(), reader.u8()};
    entity.owner = reader.i32(); entity.health = reader.i32(); entity.max_health = reader.i32();
    entity.move_wait = reader.i32(); entity.move_interval = reader.i32();
    entity.attack_wait = reader.i32(); entity.attack_interval = reader.i32();
    entity.use_flash = reader.i32(); entity.block_ticks = reader.i32();
    entity.burn_ticks = reader.i32(); entity.freeze_ticks = reader.i32();
    entity.scorch_ticks = reader.i32(); entity.fire_dim_ticks = reader.i32();
    entity.fire_tramples = reader.u8();
    entity.sleep_ticks = reader.i32(); entity.stun_ticks = reader.i32();
    entity.script_tick = reader.i32(); entity.artifacts = reader.u32();
    entity.train_cars_left = reader.i32(); entity.spawn_wait = reader.i32();
    entity.train_origin = read_cell(reader);
    entity.entity_a = {reader.i32(), reader.u32()};
    entity.entity_b = {reader.i32(), reader.u32()};
    for (Handle handle : {entity.entity_a, entity.entity_b})
        if (handle.slot < -1 || handle.slot >= max_entities) reader.okay = false;
    entity.point_a = read_cell(reader); entity.point_b = read_cell(reader);
    entity.counter_a = reader.i32(); entity.counter_b = reader.i32();
    entity.label_a = reader.i32(); entity.label_b = reader.i32();
    entity.timer_a = reader.i32(); entity.timer_b = reader.i32();
    if (entity.timer_a < 0 || entity.timer_b < 0) reader.okay = false;
    entity.birth_tick = reader.u64();
    entity.impassable = reader.u8() != 0;
    entity.hard_blocker = reader.u8() != 0;
    entity.fixture_open = reader.u8() != 0;
    entity.inventory.selected = reader.i32();
    if (entity.inventory.selected < 0 || entity.inventory.selected >= quick_slots)
        reader.okay = false;
    for (Item& item : entity.inventory.slots) item = read_item(reader);
    entity.ground_item = read_item(reader);
    if (entity.health < 0 || entity.max_health < 0 || entity.move_wait < 0 ||
        entity.move_interval < 0 || entity.attack_wait < 0 || entity.attack_interval < 0 ||
        entity.spawn_wait < 0 || entity.owner >= 4 || entity.burn_ticks < 0 ||
        entity.freeze_ticks < 0 || entity.sleep_ticks < 0 || entity.stun_ticks < 0 ||
        entity.scorch_ticks < 0 || entity.scorch_ticks > 300 || entity.fire_tramples > 5 ||
        entity.fire_dim_ticks < 0 || entity.fire_dim_ticks > 60)
        reader.okay = false;
    return entity;
}

} // namespace

std::vector<std::uint8_t> encode_game(const Game& game) {
    PacketWriter writer;
    writer.u32(12);
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
        writer.u8(static_cast<std::uint8_t>(tile.prop.kind));
        writer.u8(tile.prop.hp); writer.u8(tile.prop.variant);
        writer.u8(static_cast<std::uint8_t>(tile.prop.broken));
    }
    const Run& run = game.run;
    writer.u8(static_cast<std::uint8_t>(run.phase));
    writer.i32(run.floor); writer.u64(run.seed);
    writer.u8(static_cast<std::uint8_t>(run.death_policy));
    writer.u8(static_cast<std::uint8_t>(run.has_key));
    writer.u8(static_cast<std::uint8_t>(run.objective));
    write_cell(writer, run.spawn); write_cell(writer, run.exit);
    writer.i32(run.roof_light_count);
    for (const StageLight& light : run.roof_lights) {
        write_cell(writer, light.cell);
        write_light(writer, light.light);
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
    return writer.bytes;
}

bool decode_game(std::span<const std::uint8_t> bytes, Game& game, std::string& error) {
    PacketReader reader{bytes};
    if (reader.u32() != 12) { error = "Snapshot version mismatch"; return false; }
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
        if (kind > static_cast<std::uint8_t>(TileKind::Ice)) reader.okay = false;
        tile.kind = static_cast<TileKind>(kind);
        tile.hp = reader.u16(); tile.water_phase = reader.u8();
        tile.max_hp = reader.u16();
        tile.break_rule = static_cast<BreakRule>(reader.u8());
        tile.required_dig_power = reader.u8();
        tile.prop.kind = static_cast<PropKind>(reader.u8());
        tile.prop.hp = reader.u8(); tile.prop.variant = reader.u8();
        tile.prop.broken = reader.u8() != 0;
        if (tile.prop.kind >= PropKind::Count ||
            (tile.prop.broken && tile.prop.hp != 0)) reader.okay = false;
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
    run.spawn = read_cell(reader); run.exit = read_cell(reader);
    run.roof_light_count = reader.i32();
    if (run.roof_light_count < 0 || run.roof_light_count > 16) reader.okay = false;
    for (StageLight& light : run.roof_lights) {
        light.cell = read_cell(reader);
        light.light = read_light(reader);
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
            if (reward.kind > RewardKind::Speed || reward.item > ItemKind::CookedMeat ||
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
                if (reward.kind > RewardKind::Speed || reward.item > ItemKind::CookedMeat ||
                    reward.artifact > ArtifactKind::FleetFeet || reward.amount < 0 ||
                    reward.attribute > ItemAttribute::Restorative)
                    reader.okay = false;
            }
        }
    }
    for (ItemKind& item : run.shop_stock) {
        item = static_cast<ItemKind>(reader.u8());
        if (item > ItemKind::CookedMeat) reader.okay = false;
    }
    for (Entity& entity : result.entities) entity = read_entity(reader);
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
