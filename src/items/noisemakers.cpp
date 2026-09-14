#include "noisemakers.hpp"
#include "../entities/hearing.hpp"
#include "../projectiles/projectile.hpp"

namespace {

constexpr RegionalItem bell{"Hand Bell", "Wake sleepers and draw curious creatures to this spot. Sound travels around corners, not through walls.",
    Sprite::HandBell, {0, 0, 10, 0, 90, PatternEffect::Utility},
    ItemAction::Material, 5, 1, false, 80, 0, 0, 0, 0, SoundId::HandBell};
constexpr RegionalItem cracker{"Firecracker", "Throw, then bang after 1.5s. Small creatures stun for 0.5s and flee; larger hunters investigate. No damage.",
    Sprite::Firecracker, {1, 4, 3, 0, 60, PatternEffect::Utility},
    ItemAction::Throw, 7, 5, true, 0, 0, 0, 0, 0, SoundId::CrackerThrow};

} // namespace

const RegionalItem* forest_noisemaker(ItemKind kind) {
    if (kind == ItemKind::HandBell) return &bell;
    if (kind == ItemKind::Firecracker) return &cracker;
    return nullptr;
}

bool use_noisemaker(Game& game, int slot, Cell direction) {
    const Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const Item item = *user.inventory.held();
    const ItemPattern pattern = item_pattern(item);
    if (item.kind == ItemKind::HandBell) {
        make_noise(game, user.cell, pattern.blast_radius);
        return true;
    }
    // FUSE: A real thrown object keeps burning after its owner leaves or dies.
    Entity* shot = get_entity(game, spawn_entity(game, EntityKind::Projectile, user.cell));
    if (shot == nullptr) return false;
    shot->label_a = static_cast<int>(ProjectileKind::Firecracker);
    shot->ground_item = item; shot->ground_item.count = 1;
    shot->counter_a = shot->attack_interval = pattern.maximum;
    shot->timer_a = 90; shot->timer_b = 8;
    shot->facing = direction; shot->point_a = user.cell;
    shot->entity_a = {slot, user.generation};
    shot->sprite = Sprite::FirecrackerLit;
    shot->light = {1, 130, {255, 168, 75}};
    emit_sound(game, SoundId::CrackerFuse, user.cell);
    return true;
}

void step_firecracker(Game& game, int slot) {
    Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    if (shot.timer_a == 0) {
        const Cell cell = shot.cell;
        const int radius = item_pattern(shot.ground_item).blast_radius;
        remove_entity(game, {slot, shot.generation});
        make_noise(game, cell, 10, radius);
        emit_sound(game, SoundId::CrackerBang, cell);
        return;
    }
    if (shot.timer_a % 30 == 0) emit_sound(game, SoundId::CrackerFuse, shot.cell);
    if (shot.counter_a == 0 || shot.timer_b > 0) return;
    const Cell next = shot.cell + shot.facing;
    if (projectile_blocked(game, next)) shot.counter_a = 0;
    else { shot.cell = next; --shot.counter_a; shot.timer_b = 8; }
    if (shot.counter_a == 0) emit_sound(game, SoundId::CrackerLand, shot.cell);
}
