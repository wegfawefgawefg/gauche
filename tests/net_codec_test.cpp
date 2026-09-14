#include "../src/net_codec.hpp"
#include "../src/projectiles/projectile.hpp"

#include <cstdio>

int main() {
    Game original;
    start_run(original, 22991);
    for (int tick = 0; tick < 120; ++tick) step_game(original, {});
    if (Entity* player = get_entity(original, original.players[0])) {
        player->encounter = {401, 17};
        player->entity_a = original.players[0];
        player->entity_b = {400, 91}; // A stale reference is valid saved behavior state.
        player->point_a = {13, 27}; player->point_b = {19, 8};
        player->counter_a = 4; player->counter_b = -8;
        player->label_a = 2; player->label_b = 3;
        player->timer_a = 75; player->timer_b = 109;
        player->burn_ticks = 17;
        player->scorch_ticks = 239;
        player->freeze_ticks = 23;
        player->vitals.chill_guard = 351;
        player->vitals.recovery = RecoveryKind::Poultice;
        player->vitals.healing_left = 9;
        player->vitals.healing_wait = 27;
        player->sleep_ticks = 31;
        player->stun_ticks = 3;
        player->artifacts = 1U << static_cast<unsigned int>(ArtifactKind::Hearth);
        player->inventory.slots[2] =
            make_item(ItemKind::Pickaxe, 1, ItemAttribute::Durable);
        player->inventory.slots[2].uses = 7;
        player->light = {9, 1700, {180, 210, 255}};
        player->self_light = {40, 60, 80};
        player->inventory.slots[2].light = {2, 500, {10, 200, 30}};
    }
    // REFERENCES: A carried or dropped half keeps its placed threshold. A reflected
    // boomerang still reserves its original owner's slot, not the new attacker's.
    const Handle threshold = spawn_entity(original, EntityKind::PocketDoor, {8, 8});
    Entity* owner = get_entity(original, original.players[0]);
    owner->inventory.slots[4] = make_item(ItemKind::PocketDoor);
    owner->inventory.slots[4].uses = 1;
    owner->inventory.slots[4].anchor = threshold;
    const Handle shot_handle = spawn_entity(original, EntityKind::Projectile, {9, 8});
    Entity* shot = get_entity(original, shot_handle);
    shot->label_a = static_cast<int>(ProjectileKind::Boomerang);
    shot->entity_a = spawn_entity(original, EntityKind::Wolf, {10, 8});
    shot->entity_b = original.players[0];
    shot->ground_item = make_item(ItemKind::Boomerang);
    owner->inventory.slots[3] = shot->ground_item;
    owner->inventory.slots[3].flight = shot_handle;
    Entity* dropped = get_entity(original, spawn_entity(original, EntityKind::GroundItem, {7, 8}));
    dropped->ground_item = owner->inventory.slots[4];
    dropped->ground_item.anchor = spawn_entity(original, EntityKind::PocketDoor, {7, 9});
    original.run.roof_lights[0].light = {6, 1400, {255, 140, 70}};
    for (Entity& fire : original.entities)
        if (fire.kind == EntityKind::Campfire) {
            fire.fire_tramples = 3;
            fire.fire_dim_ticks = 47;
        }
    original.stage.tiles[0] = {TileKind::Wall, 37, 0, 125, BreakRule::DigRequired, 3};
    original.stage.tiles[3].kind = TileKind::Ice;
    original.stage.tiles[3].surface.gritted = true;
    original.stage.tiles[3].thaw_kind = TileKind::IceHole;
    original.stage.tiles[3].freeze_ticks = 319;
    original.stage.tiles[4].kind = TileKind::Ice;
    original.stage.tiles[4].thaw_kind = TileKind::Spring;
    original.stage.tiles[4].freeze_ticks = 77;
    original.stage.tiles[5].surface.warmth_ticks = 117;
    original.stage.tiles[6].prop = {PropKind::IceBlock, 23, 0, false, 411};
    original.stage.tiles[9].prop = {PropKind::AlarmClock, 13, 1, false, 237};
    original.stage.tiles[8].prop = {PropKind::BeamLamp, 19, 1, false, 0, true};
    original.stage.tiles[7].material = TileMaterial::Ice;
    owner->inventory.slots[0] = make_item(ItemKind::Chisel, 1, ItemAttribute::Fragile);
    owner->inventory.slots[0].durability = 13;
    owner->inventory.slots[0].muffled_uses = 4;
    owner->inventory.slots[5] = make_item(ItemKind::GritPouch, 1, ItemAttribute::Big);
    owner->inventory.slots[5].uses = 2;
    original.stage.tiles[1].prop = {PropKind::Crate, 7, 42, false};
    original.stage.tiles[2].prop = {PropKind::Puffball, 0, 9, true};
    original.run.pending_count[0] = 1;
    original.run.pending_offers[0][0][0] =
        {RewardKind::Item, ItemKind::RocketLauncher, ArtifactKind::None,
         1, ItemAttribute::Big};
    const auto encoded = encode_game(original);
    Game restored;
    std::string error;
    if (!decode_game(encoded, restored, error) || game_hash(restored) != game_hash(original) ||
        encode_game(restored) != encoded) {
        std::fprintf(stderr, "snapshot round-trip failed: %s\n", error.c_str());
        return 1;
    }
    for (std::size_t size = 0; size < encoded.size(); size += 17) {
        Game bad;
        if (decode_game(std::span<const std::uint8_t>{encoded.data(), size}, bad, error)) {
            std::fprintf(stderr, "truncated snapshot accepted at %zu\n", size);
            return 1;
        }
    }
    PacketWriter writer;
    Input input;
    input.move = {-1, 0};
    input.aim = {2, -1};
    input.use = true;
    input.select = 5;
    writer.input(input);
    PacketReader reader{writer.bytes};
    if (reader.input() != input || !reader.finished()) {
        std::fputs("input codec failed\n", stderr);
        return 1;
    }
    std::puts("snapshot codec passed");
    return 0;
}
