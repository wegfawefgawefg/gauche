#include "optics.hpp"
#include "../props/growth.hpp"

namespace {

constexpr RegionalItem carbine{"Lens Carbine", "Instant light beam. Mirrors turn it; lenses split it. Opaque cover stops it. Returned beams can hit you.",
    Sprite::LensCarbine, {1, 12, 0, 18, 24, PatternEffect::Damage, true},
    ItemAction::Gun, 40, 1, false, 0, 3, 6, 80, 0, SoundId::LensFire};
constexpr RegionalItem mirror{"Mirror Shard", "Place a mirror, 8 HP. Turns beams only. Rotate with a shard or empty hand. Bullets break it.",
    Sprite::MirrorShard, {1, 1, 0, 0, 18, PatternEffect::Utility},
    ItemAction::Material, 16, 3, true, 0, 0, 0, 0, 0, SoundId::OpticPlace};
constexpr RegionalItem lens{"Crystal Lens", "Place a fragile lens, 18 HP. Splits light into two sideways beams at half damage. Bullets break it.",
    Sprite::CrystalLens, {1, 1, 0, 0, 24, PatternEffect::Utility},
    ItemAction::Material, 30, 1, true, 0, 0, 0, 0, 0, SoundId::OpticPlace};

constexpr RegionalItem felt{"Black Felt", "Cover optics: blocks beams and lamp light. Burns away. Tear off with felt or empty hand; destroys cloth.",
    Sprite::BlackFelt, {1, 1, 0, 0, 18, PatternEffect::Utility},
    ItemAction::Material, 7, 3, true, 0, 0, 0, 0, 0, SoundId::FeltCover};

constexpr RegionalItem bomb{"Prism Bomb", "Throw, then 1.5s fuse. Bursts four light beams. Mirrors turn them; lenses split them. Step out of their lanes.",
    Sprite::PrismBomb, {3, 3, 4, 16, 45, PatternEffect::Damage, false, 0, 0, false, false, true},
    ItemAction::Throw, 28, 2, true, 0, 0, 0, 0, 0, SoundId::PrismThrow};

} // namespace

const RegionalItem* optics_item(ItemKind kind) {
    switch (kind) {
    case ItemKind::BlackFelt: return &felt;
    case ItemKind::PrismBomb: return &bomb;
    case ItemKind::LensCarbine: return &carbine;
    case ItemKind::MirrorShard: return &mirror;
    case ItemKind::CrystalLens: return &lens;
    default: return nullptr;
    }
}

bool place_optic(Game& game, int slot, Cell direction) {
    const Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    const PropKind kind = actor.inventory.held()->kind == ItemKind::MirrorShard ? PropKind::MirrorShard : PropKind::CrystalLens;
    return plant_prop(game, actor.cell + direction, kind);
}

bool rotate_mirror(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    Item& held = *actor.inventory.held();
    // EMPTY HAND: The last shard was consumed by placement; it must still be rotatable.
    if (held.kind != ItemKind::None && held.kind != ItemKind::MirrorShard) return false;
    if (held.cooldown > 0) return false;
    Tile* tile = game.stage.at(actor.cell + actor.facing);
    if (!tile || tile->prop.broken || tile->prop.covered || tile->prop.kind != PropKind::MirrorShard) return false;
    tile->prop.variant ^= 1;
    held.cooldown = 12;
    emit_sound(game, SoundId::OpticTurn, actor.cell + actor.facing);
    return true;
}
