#include "mixtures.hpp"
#include "../entities/hearing.hpp"
#include "../projectiles/projectile.hpp"
#include "../surfaces/interaction.hpp"
#include "../props/interaction.hpp"

#include <algorithm>

namespace {

constexpr RegionalItem stink{"Stink Bomb", "Throw a lingering scent. Hungry animals and scavengers sniff it out. Water or fire clears it; no damage.",
    Sprite::StinkBomb, {1, 4, 1, 0, 45, PatternEffect::Utility},
    ItemAction::Throw, 7, 4, true, 0, 0, 0, 0, 0, SoundId::StinkThrow};
constexpr RegionalItem fruit{"Rotten Fruit", "Throw foul bait: contact causes nausea. Or eat it for 3 HP, then 6s of nausea. Water washes rot away.",
    Sprite::RottenFruit, {1, 4, 1, 0, 45, PatternEffect::Damage},
    ItemAction::Throw, 2, 8, true, 0, 0, 0, 0, 0, SoundId::FruitThrow};
constexpr RegionalItem pitch{"Pitch Bomb", "A 2s fuse, then 12 damage and burning sticky sap. Ignites growth; dangerous to friends. Water quenches it.",
    Sprite::PitchBomb, {2, 6, 2, 12, 75, PatternEffect::Damage},
    ItemAction::Throw, 18, 3, true, 0, 0, 0, 0, 0, SoundId::PitchThrow};

} // namespace

const RegionalItem* forest_mixture(ItemKind kind) {
    switch (kind) {
    case ItemKind::StinkBomb: return &stink;
    case ItemKind::RottenFruit: return &fruit;
    case ItemKind::PitchBomb: return &pitch;
    default: return nullptr;
    }
}

bool throw_mixture(Game& game, int slot, Cell direction) {
    const Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const Item item = *user.inventory.held();
    const ItemPattern pattern = item_pattern(item);
    Entity* shot = get_entity(game, spawn_entity(game, EntityKind::Projectile, user.cell));
    if (shot == nullptr) return false;
    shot->label_a = static_cast<int>(ProjectileKind::Mixture);
    shot->ground_item = item; shot->ground_item.count = 1;
    shot->counter_a = shot->attack_interval = pattern.maximum;
    shot->timer_a = item.kind == ItemKind::PitchBomb ? 120 : pattern.maximum * 8 + 2;
    shot->timer_b = 8; shot->point_a = user.cell; shot->facing = direction;
    shot->entity_a = {slot, user.generation};
    shot->sprite = item_sprite(item);
    if (item.kind == ItemKind::PitchBomb) {
        shot->sprite = Sprite::PitchBombLit;
        shot->light = {2, 170, {255, 154, 64}};
        emit_sound(game, SoundId::PitchFuse, user.cell);
    }
    return true;
}

std::vector<Cell> mixture_cells(const Game& game, const Item& item, Cell center) {
    const int radius = item_pattern(item).blast_radius;
    std::vector<Cell> cells = audible_cells(game, center, radius);
    if (item.kind != ItemKind::PitchBomb) return cells;
    // EDGES: Pitch hits exposed cover, but breaking it cannot expand this same splash.
    const auto open = cells;
    for (Cell cell : open)
        for (Cell side : {Cell{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
            const Cell edge = cell + side;
            if (game.stage.in_bounds(edge) && distance(edge, center) <= radius &&
                std::find(cells.begin(), cells.end(), edge) == cells.end()) cells.push_back(edge);
        }
    return cells;
}

void splash_mixture(Game& game, const Item& item, Cell center, Cell source) {
    const ItemPattern pattern = item_pattern(item);
    emit_sound(game, item.kind == ItemKind::PitchBomb ? SoundId::PitchBurst :
        item.kind == ItemKind::StinkBomb ? SoundId::StinkBreak : SoundId::FruitSplat, center);
    const auto cells = mixture_cells(game, item, center);
    for (Cell cell : cells) {
        Tile* tile = game.stage.at(cell);
        if (tile == nullptr) continue;
        if (item.kind == ItemKind::StinkBomb) {
            if (!surface_wet(*tile) && tile->surface.fire_ticks == 0)
                tile->surface.scent_ticks = std::max<std::uint16_t>(tile->surface.scent_ticks, 600);
        } else if (item.kind == ItemKind::RottenFruit) {
            pour_surface(game, cell, LiquidKind::Rot, 600);
        } else {
            hit_prop(game, cell, pattern.damage * 2, source);
            pour_surface(game, cell, LiquidKind::Sap, 900);
            ignite_surface(game, cell);
            if (tile->surface.liquid == LiquidKind::Sap) tile->surface.liquid_ticks = 900;
            for (int index = 0; index < max_entities; ++index) {
                Entity& actor = game.entities[static_cast<std::size_t>(index)];
                if (actor.cell != cell || actor.health <= 0) continue;
                damage_entity(game, index, pattern.damage, source, false);
                contact_surface(game, index);
            }
        }
    }
}

void step_mixture(Game& game, int slot) {
    Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    const bool pitch_bomb = shot.ground_item.kind == ItemKind::PitchBomb;
    if (shot.timer_a == 0 || (!pitch_bomb && shot.counter_a == 0)) {
        const Item item = shot.ground_item;
        const Cell center = shot.cell, source = shot.point_a;
        remove_entity(game, {slot, shot.generation});
        splash_mixture(game, item, center, source);
        return;
    }
    if (pitch_bomb && shot.timer_a % 30 == 0) emit_sound(game, SoundId::PitchFuse, shot.cell);
    if (shot.counter_a == 0 || shot.timer_b > 0) return;
    const Cell next = shot.cell + shot.facing;
    if (projectile_blocked(game, next)) shot.counter_a = 0;
    else { shot.cell = next; --shot.counter_a; shot.timer_b = 8; }
    if (pitch_bomb && shot.counter_a == 0) emit_sound(game, SoundId::PitchLand, shot.cell);
}

bool eat_rotten_fruit(Game& game, int slot) {
    Entity& user = game.entities[static_cast<std::size_t>(slot)];
    Item& item = *user.inventory.held();
    if (item.kind != ItemKind::RottenFruit || item.count <= 0 || item.cooldown > 0 ||
        user.health <= 0 || user.health >= user.max_health) return false;
    user.health = std::min(user.max_health, user.health + 3);
    apply_nausea(user, 360);
    item.cooldown = 90;
    user.use_flash = 8;
    if (--item.count == 0) item = {};
    emit_sound(game, SoundId::FruitMunch, user.cell);
    return true;
}
